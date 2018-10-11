#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\Command.hpp"
#include <string>

NetSession* g_theNetSession = nullptr;
std::map<std::string, NetMessageDefinition*> NetSession::s_registeredMessageDefinitions;

//  =============================================================================
NetSession::NetSession()
{

}

//  =============================================================================
NetSession::~NetSession()
{
	delete(m_socket);
	m_socket = nullptr;

	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		//disconnect m_connections[connectionIndex]
		m_connections[connectionIndex]->FlushOutgoingMessages();
		delete(m_connections[connectionIndex]);
		m_connections[connectionIndex] = nullptr;
	}
}

//  =============================================================================
NetSession* NetSession::GetInstance()
{
	return g_theNetSession;
}

//  =============================================================================
NetSession* NetSession::CreateInstance()
{
	if (g_theNetSession == nullptr)
	{
		g_theNetSession = new NetSession();
	}
	return g_theNetSession;
}

//  =========================================================================================
void NetSession::Update()
{
	ProcessOutgoingMessages();
	ProcessIncomingMessages();
}

//  =========================================================================================
void NetSession::Startup()
{
	m_socket = new UDPSocket();
	m_socket->BindToPort(GAME_PORT);

	//register messages
	RegisterMessageDefinition("ping", OnPing);
	RegisterMessageDefinition("pong", OnPong);
	RegisterMessageDefinition("add", OnAdd);
	RegisterMessageDefinition("add_response", OnAddResponse);

	LockMessageDefinitionRegistration();

	//register app commands
	CommandRegister("add_connection", CommandRegistration(AddConnectionToIndex, ": Attempt to add connection at index: idx IP", "Successfully added connection!"));
	CommandRegister("send_ping", CommandRegistration(SendPing, ": Send ping to index: idx", ""));
	CommandRegister("send_add", CommandRegistration(SendAdd, ": Send add request to index: idx var1 var2", ""));
}

//  =============================================================================
bool NetSession::BindPort(uint port, uint range)
{
	m_socket = new UDPSocket();

	NetAddress address;

	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(port);

	address.GetMyHostAddress(&addr, &addrLength, portString.c_str());
	address.FromSockAddr(&addr);

	return m_socket->Bind(address, range);
}

//  =============================================================================
bool NetSession::AddConnection(uint8_t connectionIndex, NetAddress* address)
{
	if (m_connections.size() > connectionIndex)
	{
		if(m_connections[connectionIndex] != nullptr)
			return false;
	}
	else if(m_connections.size() < connectionIndex)
	{
		return false;
	}

	NetConnection* connection = new NetConnection();
	connection->m_index = connectionIndex;
	connection->m_address = address;	

	m_connections.push_back(connection);

	//if the connection we are mapping is our own netsession, we need to store off our own index for ease of access
	if (m_sessionConnectionIndex == UINT8_MAX)
	{
		if (*address == m_socket->m_address)
		{
			m_sessionConnectionIndex = connectionIndex;
		}
	}

	return true;
}

//  =============================================================================
void NetSession::ProcessIncomingMessages()
{
	std::vector<NetPacket> packetsToProcess;

	size_t receivedAmount = INT_MAX;

	NetPacket receivedPacket = NetPacket(UINT8_MAX, 0);
	receivedPacket.ExtendBufferSize(PACKET_MTU);
	NetAddress senderAddress;

	//read until no more packets are in the socket
	while (receivedAmount > 0)
	{
		receivedAmount = m_socket->Receive(&senderAddress, receivedPacket.GetBuffer(), PACKET_MTU);
		
		if (receivedAmount != 0)
		{
			//check packet validity
			if (!receivedPacket.CheckIsValid())
			{
				continue;
			}

			//get the header data
			receivedPacket.ReadBytes(receivedPacket.m_packetHeader, sizeof(NetPacketHeader), false);

			NetConnection* connection = GetConnectionById(receivedPacket.m_packetHeader->m_senderIndex);

			//we don't have a connection on this index (probably invalid)
			if (connection == nullptr)
			{
				connection = new NetConnection();
				connection->m_address = &senderAddress;
			}

			for(int messageIndex = 0; messageIndex < (int)receivedPacket.m_packetHeader->m_messageCount; ++messageIndex)
			{
				uint16_t totalSize = UINT16_MAX;
				uint8_t callbackId = UINT8_MAX;

				receivedPacket.ReadBytes(&totalSize, sizeof(uint16_t), false);
				receivedPacket.ReadBytes(&callbackId, sizeof(uint8_t), false);

				NetMessageDefinition* definition = GetRegisteredDefinitionById((int)callbackId);

				if (definition != nullptr)
				{
					NetMessage* message = new NetMessage(definition->m_callbackName);

					//if there is more to the message we must read that in before calling the return function

					size_t remainingAmountToRead = totalSize - sizeof(NetMessageHeader);
					if (remainingAmountToRead > 0)
					{
						message->ExtendBufferSize(remainingAmountToRead);
						receivedPacket.ReadBytes(message->GetBuffer(), remainingAmountToRead, false);
						message->SetWrittenByteCountToBufferSize();
					}			

					definition->m_callback(*message, connection);

					//cleanup
					delete(message);
					message = nullptr;
				}				

				delete(definition);
				definition = nullptr;
			}
			
			//cleanup
			connection = nullptr;			
			
			//packetsToProcess.push_back(receivedPacket);			
		}
	}
}

//  =============================================================================
void NetSession::ProcessOutgoingMessages()
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		m_connections[connectionIndex]->FlushOutgoingMessages();
	}
}

//  =============================================================================
bool NetSession::SendMessageWithoutConnection(NetMessage* message, NetConnection* connection)
{
	bool success = false;
	NetSession* theNetSession = NetSession::GetInstance();

	bool areMessagesPacked = false;
	int currentMessageIndex = 0;

	std::vector<NetPacket*> packetsToSend;
	NetPacket* packet = new NetPacket(theNetSession->m_sessionConnectionIndex, 0);
	packet->WriteUpdatedHeaderData();

	if (packet->GetBufferSize() + message->GetWrittenByteCount() <= PACKET_MTU)
	{
		//write message to packet
		packet->WriteMessage(*message);
		packetsToSend.push_back(packet);
	}

	//send each packet
	for(int packetIndex = 0; packetIndex < (int)packetsToSend.size(); ++packetIndex)
	{
		size_t amountSent = theNetSession->m_socket->SendTo(*connection->m_address, packetsToSend[packetIndex]->GetBuffer(), packetsToSend[packetIndex]->GetWrittenByteCount());
	
		if(amountSent > 0)
			success = true;
		else
			success = false;
	}

	//cleanup outgoing message queue
	packetsToSend.clear();

	delete(packet);
	packet = nullptr;
	theNetSession = nullptr;

	return success;
}

//  =============================================================================
bool NetSession::RegisterMessageDefinition(const std::string& name, NetMessageCallback callback)
{
	bool success = false;

	if (!m_isDefinitionRegistrationLocked)
	{
		NetMessageDefinition* definition = new NetMessageDefinition();
		definition->m_callbackName = name;
		definition->m_callback = callback;

		s_registeredMessageDefinitions.insert(std::pair<std::string, NetMessageDefinition*>(name, definition));
		success = true;
	}

	return success;
}

//  =========================================================================================
void NetSession::LockMessageDefinitionRegistration()
{
	m_isDefinitionRegistrationLocked = true;

	int idCount = 0;
	for (std::map<std::string, NetMessageDefinition*>::iterator defIterator = s_registeredMessageDefinitions.begin(); defIterator != s_registeredMessageDefinitions.end(); ++defIterator)
	{
		defIterator->second->m_callbackId = idCount;
		++idCount;
	}
}

//  =========================================================================================
NetConnection* NetSession::GetConnectionById(uint8_t id)
{
	if (id < m_connections.size() )
	{
		return m_connections[(int)id];
	}

	return nullptr;	
}

//  =========================================================================================
//  Callback
//  =========================================================================================

NetMessageDefinition* GetRegisteredDefinitionById(int id)
{
	std::map<std::string, NetMessageDefinition*>::iterator definitionIterator = NetSession::s_registeredMessageDefinitions.begin();
	std::advance(definitionIterator, id);
	if (definitionIterator != NetSession::s_registeredMessageDefinitions.end())
	{
		return definitionIterator->second;
	}

	return nullptr;
}

//  =========================================================================================
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string & name)
{
	std::map<std::string, NetMessageDefinition*>::iterator definitionIterator;
	definitionIterator = NetSession::s_registeredMessageDefinitions.find(name);
	if (definitionIterator != NetSession::s_registeredMessageDefinitions.end())
	{
		return definitionIterator->second;
	}

	return nullptr;
}

//  =========================================================================================
NetMessageCallback GetRegisteredCallbackByName(const std::string& name)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionByName(name);
	if(definition != nullptr)
		return definition->m_callback;

	return nullptr;
}

//  =========================================================================================
NetMessageCallback GetRegisteredNetCallbackById(int id)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionById(id);
	if (definition != nullptr)
		return definition->m_callback;

	return nullptr;	
}

//  =========================================================================================
//  Console Commands
//  =========================================================================================
void AddConnectionToIndex(Command& cmd)
{
	int index = cmd.GetNextInt();
	std::string address = cmd.GetNextString();

	NetSession* theNetSession = NetSession::GetInstance();

	if (theNetSession->m_connections.size() != index)
	{
		DevConsolePrintf(Rgba::RED, "Requested index %i is invalid", index);
	}

	else if (IsStringNullOrEmpty(address))
	{
		DevConsolePrintf(Rgba::RED, "Address is invalid");
	}		
	else
	{
		NetAddress* netAddress = new NetAddress(address.c_str());
		if (netAddress == nullptr)
		{
			DevConsolePrintf(Rgba::RED, "Address is invalid");
		}
		else
		{
			NetSession::GetInstance()->AddConnection(index, netAddress);
			DevConsolePrintf("Successfully added address (%s) at index %i", address.c_str(), index);
		}	
		netAddress = nullptr;
	}

	//cleanup
	theNetSession = nullptr;
}

//  =========================================================================================
void SendPing(Command& cmd)
{
	uint8_t index = (uint8_t)cmd.GetNextInt();

	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)index);
	if (connection == nullptr)
	{
		DevConsolePrintf("No connection at index %u", index);
		return;
	}

	NetMessage* message = new NetMessage("ping");

	std::string pingString = cmd.GetNextString(); 
	if (pingString == "")
	{
		pingString = "ping";
	}

	bool success = message->WriteString(pingString.c_str());

	if (!success)
	{
		DevConsolePrintf(Rgba::RED, "Message length incompatible with MTU size.");
		delete(message);
	}
	else
	{
		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);
	}
	
	//cleanup
	message = nullptr;
	theNetSession = nullptr;
}

//  =========================================================================================
void SendAdd(Command& cmd)
{
	int index = cmd.GetNextInt();

	float parameter1 = cmd.GetNextFloat();
	float parameter2 = cmd.GetNextFloat();

	if (index == INT_MAX) {
		DevConsolePrintf(Rgba::RED, "Must provide an index." ); 
		return; 
	}
	NetSession* theNetSession = NetSession::GetInstance();
	
	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)index);
	if (connection == nullptr) 
	{
		DevConsolePrintf( "No connection at index %u", index ); 
		return; 
	}

	NetMessage* message = new NetMessage("add");

	//add float data
	bool success = message->WriteBytes(sizeof(float), &parameter1, false);
	success = message->WriteBytes(sizeof(float), &parameter2, false);

	if (!success)
	{
		DevConsolePrintf(Rgba::RED, "Message length incompatible with MTU size.");
		delete(message);
	}
	else
	{
		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);
	}

	message = nullptr;
	theNetSession = nullptr;
}


//  =============================================================================
//	NetMessage Definition Callbacks
//  =============================================================================
bool OnPing(NetMessage& message, NetConnection* fromConnection)
{
	DevConsolePrintf("Received PING from %s (Connection:%i)", fromConnection->m_address->ToString().c_str(), (int)fromConnection->m_index);

	NetMessage* pongMessage = new NetMessage("pong");

	// messages are sent to connections (not sessions)
	if (fromConnection->m_index != UINT8_MAX)
	{
		fromConnection->QueueMessage(pongMessage);
	}
	else
	{
		NetSession::GetInstance()->SendMessageWithoutConnection(pongMessage, fromConnection);
	}			

	return true;
}

//  =============================================================================
bool OnPong(NetMessage& message, NetConnection* fromConnection)
{
	DevConsolePrintf("Received PONG from %s (Connection:%i)", fromConnection->m_address->ToString().c_str(), (int)fromConnection->m_index);
	return true;
}

//  =============================================================================
bool OnAdd(NetMessage& message, NetConnection* fromConnection)
{
	float parameter1 = 0;
	float parameter2 = 0;

	bool success = message.ReadBytes(&parameter1, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param1", fromConnection->m_index);
		return success;
	}

	success = message.ReadBytes(&parameter2, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param2", fromConnection->m_index);
		return success;
	}

	float sum = parameter1 + parameter2;
	DevConsolePrintf("Received ADD function from connection %i: %f + %f = %f", fromConnection->m_index
	,parameter1
	,parameter2
	,sum);

	//send 
	NetMessage* responseMessage = new NetMessage("add");

	//add float data
	success = responseMessage->WriteBytes(sizeof(float), &parameter1, false);
	success = responseMessage->WriteBytes(sizeof(float), &parameter2, false);
	success = responseMessage->WriteBytes(sizeof(float), &sum, false);

	if (!success)
	{
		DevConsolePrintf(Rgba::RED, "Message length incompatible with MTU size.");
		delete(responseMessage);
	}
	else
	{
		// messages are sent to connections (not sessions)
		if (fromConnection->m_index != UINT8_MAX)
		{
			fromConnection->QueueMessage(responseMessage);
		}
		else
		{
			NetSession::GetInstance()->SendMessageWithoutConnection(responseMessage, fromConnection);
		}		
	}

	return true;
}

//  =============================================================================
bool OnAddResponse(NetMessage& message, NetConnection* fromConnection)
{
	float parameter1 = 0;
	float parameter2 = 0;
	float sum = 0;

	bool success = message.ReadBytes(&parameter1, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param1", fromConnection->m_index);
		return success;
	}

	success = message.ReadBytes(&parameter2, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param2", fromConnection->m_index);
		return success;
	}

	success = message.ReadBytes(&sum, sizeof(float), false);

	DevConsolePrintf("Received ADD response from connection %i: %f + %f = %f", fromConnection->m_index
		,parameter1
		,parameter2
		,sum);

	return true;
}




