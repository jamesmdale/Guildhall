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

			for(int messageIndex = 0; messageIndex < (int)receivedPacket.m_packetHeader->m_messageCount; ++messageIndex)
			{
				uint16_t totalSize = UINT16_MAX;
				uint8_t callbackId = UINT8_MAX;

				receivedPacket.ReadBytes(&totalSize, sizeof(uint16_t), false);
				receivedPacket.ReadBytes(&callbackId, sizeof(uint8_t), false);

				NetMessageDefinition* definition = GetRegisteredDefinitionById((int)callbackId);
				NetMessage* message = new NetMessage(definition->m_callbackName);

				//if there is more to the message we must read that in before calling the return function

				size_t remainingAmountToRead = totalSize - sizeof(uint16_t) - sizeof(uint8_t);
				if (remainingAmountToRead > 0)
				{
					message->ExtendBufferSize(remainingAmountToRead);
					receivedPacket.ReadBytes(message->GetBuffer(), remainingAmountToRead, false);
				}			

				definition->m_callback(*message, connection);

				//cleanup
				delete(message);
				message = nullptr;

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
	message->InitializeHeaderData();

	//now that the message is complete, write the final size into the header
	bool success = message->WriteFinalSizeToHeader();

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

	NetMessage* message = new NetMessage("ping");

	// messages are sent to connections (not sessions)
	connection->QueueMessage( message ); 

	message = nullptr;
	theNetSession = nullptr;
}

//  =============================================================================
//	NetMessage Definition Callbacks
//  =============================================================================
bool OnPing(const NetMessage& message, NetConnection* fromConnection)
{
	DevConsolePrintf("Received PING from %s (Connection:%i)", fromConnection->m_address->ToString().c_str(), (int)fromConnection->m_index);

	NetMessage* pongMessage = new NetMessage("pong");
	pongMessage->InitializeHeaderData();

	//now that the message is complete, write the final size into the header
	bool success = pongMessage->WriteFinalSizeToHeader();

	if (!success)
	{
		DevConsolePrintf(Rgba::RED, "Message length incompatible with MTU size.");
		delete(pongMessage);
	}
	else
	{
		// messages are sent to connections (not sessions)
		fromConnection->QueueMessage(pongMessage);
	}

	return true;
}

//  =============================================================================
bool OnPong(const NetMessage& message, NetConnection* fromConnection)
{
	DevConsolePrintf("Received PONG from %s (Connection:%i)", fromConnection->m_address->ToString().c_str(), (int)fromConnection->m_index);
	return true;
}

//  =============================================================================
bool OnAdd(const NetMessage& message, NetConnection* fromConnection)
{
	return true;
}




