#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Time\Time.hpp"
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
	CheckDelayedPackets();	
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
	RegisterMessageDefinition("heartbeat", OnHeartbeat);

	LockMessageDefinitionRegistration();

	//register app commands
	CommandRegister("add_connection", CommandRegistration(AddConnectionToIndex, ": Attempt to add connection at index: idx IP", "Successfully added connection!"));
	CommandRegister("send_ping", CommandRegistration(SendPing, ": Send ping to index: idx", ""));
	CommandRegister("send_add", CommandRegistration(SendAdd, ": Send add request to index: idx var1 var2", ""));
	CommandRegister("send_multi_ping", CommandRegistration(SendMultiPing, ": Send two pings to index (tests multi message in single packet): idx", ""));
	CommandRegister("net_sim_loss", CommandRegistration(SetNetSimLoss, ": Set the simulated loss: amount (value 0.0 to 1.0)", ""));
	CommandRegister("net_sim", CommandRegistration(SetNetSimLag, ": Set the simulated network latency (in MS): min max ", ""));
	CommandRegister("net_set_heart_rate", CommandRegistration(SetGlobalHeartRate, ": Set the heartbeat rate for all connections: rate", ""));
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

//  =========================================================================================
void NetSession::CheckHeartbeats()
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		if (m_connections[connectionIndex]->m_heartbeatTimer->ResetAndDecrementIfElapsed())
		{
			SendHeartBeat(m_connections[connectionIndex]->m_index);
		}
	}
}

//  =========================================================================================
void NetSession::SendHeartBeat(int index)
{
	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)index);
	if (connection == nullptr)
	{
		DevConsolePrintf("No connection at index %u", index);
		return;
	}

	NetMessage* message = new NetMessage("heartbeat");

	// messages are sent to connections (not sessions)
	connection->QueueMessage(message);	

	message = nullptr;
	theNetSession = nullptr;
}

//  =============================================================================
void NetSession::ProcessIncomingMessages()
{
	std::vector<NetPacket> packetsToProcess;

	size_t receivedAmount = INT_MAX;
	NetAddress senderAddress;

	//read until no more packets are in the socket
	while (receivedAmount > 0)
	{
		void* buffer = malloc(PACKET_MTU);
		receivedAmount = m_socket->Receive(&senderAddress, buffer, PACKET_MTU);
		
		if (receivedAmount != 0)
		{
			NetPacket* receivedPacket = new NetPacket();
			receivedPacket->ExtendBufferSize(PACKET_MTU);
			receivedPacket->WriteBytes(receivedAmount, buffer, false);

			//check packet validity
			if (!receivedPacket->CheckIsValid())
			{
				delete(receivedPacket);
				receivedPacket = nullptr;

				continue;
			}

			//decide whether to randomly toss packet out
			if (GetRandomFloatZeroToOne() <= m_simulationLossAmount)
			{
				delete(receivedPacket);
				receivedPacket = nullptr;

				continue;
			}
			else
			{
				//store the packet in a vector with a generated latency
				float delayAmountInMilliseconds = GetRandomFloatInRange((float)m_minAddedLatencyInMilliseconds, (float)m_maxAddedLatencyInMilliseconds);

				uint64_t hpcDelay = SecondsToPerformanceCounter(delayAmountInMilliseconds/1000.f);

				DelayedReceivedPacket* delayedPacket = new DelayedReceivedPacket();
				delayedPacket->m_timeToProcess = GetMasterClock()->GetLastHPC() + hpcDelay;
				delayedPacket->m_packet = receivedPacket;
				delayedPacket->m_senderAddress = senderAddress;

				m_delayedPackets.push_back(delayedPacket);
			}
		}
		free(buffer);
	}
}

//  =============================================================================
void NetSession::CheckDelayedPackets()
{
	for (int delayedPacketIndex = 0; delayedPacketIndex < (int)m_delayedPackets.size(); ++delayedPacketIndex)
	{
		if (GetMasterClock()->GetLastHPC() > m_delayedPackets[delayedPacketIndex]->m_timeToProcess)
		{
			ProcessDelayedPacket(m_delayedPackets[delayedPacketIndex]);
			m_delayedPackets.erase(m_delayedPackets.begin() + delayedPacketIndex);
			delayedPacketIndex--;
		}
	}
}

//  =============================================================================
void NetSession::ProcessDelayedPacket(DelayedReceivedPacket* packet)
{
	//get the header data
	packet->m_packet->ReadBytes(&packet->m_packet->m_packetHeader, sizeof(NetPacketHeader), false);

	NetConnection* connection = GetConnectionById(packet->m_packet->m_packetHeader.m_senderIndex);

	//we don't have a connection on this index (probably invalid)
	if (connection == nullptr)
	{
		connection = new NetConnection();
		connection->m_address = &packet->m_senderAddress;
	}

	for(int messageIndex = 0; messageIndex < (int)packet->m_packet->m_packetHeader.m_messageCount; ++messageIndex)
	{
		uint16_t totalSize = UINT16_MAX;
		uint8_t callbackId = UINT8_MAX;
		
		packet->m_packet->ReadBytes(&totalSize, sizeof(uint16_t), false);
		packet->m_packet->ReadBytes(&callbackId, sizeof(uint8_t), false);

		NetMessageDefinition* definition = GetRegisteredDefinitionById((int)callbackId);

		if (definition != nullptr)
		{
			NetMessage* message = new NetMessage(definition->m_callbackName);

			//if there is more to the message we must read that in before calling the return function

			size_t remainingAmountToRead = totalSize - sizeof(NetMessageHeader);
			if (remainingAmountToRead > 0)
			{
				message->ExtendBufferSize(remainingAmountToRead);
				packet->m_packet->ReadBytes(message->GetBuffer(), remainingAmountToRead, false);
				message->SetWrittenByteCountToBufferSize();
			}			

			definition->m_callback(*message, connection);

			//cleanup
			delete(message);
			message = nullptr;
		}				

		definition = nullptr;
	}

	//cleanup
	connection = nullptr;			

	//packetsToProcess.push_back(receivedPacket);			
}

//  =============================================================================
void NetSession::ProcessOutgoingMessages()
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		m_connections[connectionIndex]->FlushOutgoingMessages();
		m_connections[connectionIndex]->SendPackets();
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
void NetSession::SetHeartbeatRate(float hertz)
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		if (hertz == 0)
		{
			m_connections[connectionIndex]->m_heartbeatTimer->SetTimer(0);
		}
		else
		{
			m_connections[connectionIndex]->m_heartbeatTimer->SetTimer(1.f / hertz);
		}
		
	}
}

//  =============================================================================
// Simulated Latency/Etc =============================================================================
//  =============================================================================
void NetSession::SetSimulatedLossAmount(float lossAmount)
{
	m_simulationLossAmount = lossAmount;
}

//  =============================================================================
void NetSession::SetSimulatedLatency(uint minLatencyInMilliseconds, uint maxLatencyInMilliseconds)
{
	m_minAddedLatencyInMilliseconds = minLatencyInMilliseconds;
	m_maxAddedLatencyInMilliseconds = maxLatencyInMilliseconds;
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

//  =============================================================================
void SendMultiPing(Command & cmd)
{
	uint8_t index = (uint8_t)cmd.GetNextInt();

	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)index);
	if (connection == nullptr)
	{
		DevConsolePrintf("No connection at index %u", index);
		return;
	}

	NetMessage* message1 = new NetMessage("ping");
	NetMessage* message2 = new NetMessage("ping");

	std::string pingString = cmd.GetNextString(); 
	if (pingString == "")
	{
		pingString = "ping";
	}

	bool success = message1->WriteString(pingString.c_str());
	success = message2->WriteString(pingString.c_str());

	if (!success)
	{
		DevConsolePrintf(Rgba::RED, "Message length incompatible with MTU size.");
		delete(message1);
		delete(message2);
	}
	else
	{
		// messages are sent to connections (not sessions)
		connection->QueueMessage(message1);
		connection->QueueMessage(message2);
	}

	//cleanup
	message1 = nullptr;
	message2 = nullptr;
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
void SetNetSimLag(Command& cmd)
{
	float minimumLatency = cmd.GetNextFloat();
	float maximumLatency = cmd.GetNextFloat();

	if (minimumLatency > maximumLatency)
	{
		DevConsolePrintf(Rgba::RED, "ERROR: Minimum latency cannot be greater than maximum latency");
		return;
	}

	NetSession::GetInstance()->SetSimulatedLatency(minimumLatency, maximumLatency);
	DevConsolePrintf("Latency set to Min:%f and Max%f", minimumLatency, maximumLatency);
}

//  =============================================================================
void SetNetSimLoss(Command& cmd)
{
	float simLoss = cmd.GetNextFloat();

	if (simLoss > 1 || simLoss < 0)
	{
		DevConsolePrintf(Rgba::RED, "ERROR: Loss should be a rate between 0.0 to 1.0");
		return;
	}

	NetSession::GetInstance()->SetSimulatedLossAmount(simLoss);
	DevConsolePrintf("Loss rate set to %f", simLoss);
}

//  =========================================================================================
void SetSessionSendRate(Command& cmd)
{
	float sendRate = cmd.GetNextFloat();

	if(sendRate < 0.f)
		sendRate = 0.f;

	NetSession* theNetSession = NetSession::GetInstance();

	//set net session sendrate
	if (sendRate > 0)
	{
		theNetSession->m_sessionSendLatencyInMilliseconds = (1.f / sendRate) * 1000.f;
	}
	else
	{
		theNetSession->m_sessionSendLatencyInMilliseconds = 0.f;
	}
	
	//set connection send rates to the netsession send rate for every rate that is faster than the new rate
	for (int connectionIndex = 0; connectionIndex < (int)theNetSession->m_connections.size(); ++connectionIndex)
	{
		if (theNetSession->m_sessionSendLatencyInMilliseconds > theNetSession->m_connections[connectionIndex]->m_connectionSendLatencyInMilliseconds)
		{
			theNetSession->m_connections[connectionIndex]->m_latencySendTimer->SetTimerInMilliseconds(theNetSession->m_sessionSendLatencyInMilliseconds);
		}
	}
}

//  =========================================================================================
void SetConnectionSendRate(Command& cmd)
{
	int connectionIndex = cmd.GetNextInt();
	float sendRate = cmd.GetNextFloat();

	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)connectionIndex);
	if (connection == nullptr)
	{
		DevConsolePrintf("No connection at index %u", connectionIndex);
	}
	else
	{
		if(sendRate < 0)
			sendRate = 0;

		//update latency
		if (sendRate > 0)
		{
			connection->m_connectionSendLatencyInMilliseconds = (1.f / sendRate) * 1000.f;
		}
		else
		{
			connection->m_connectionSendLatencyInMilliseconds = 0.f;
		}

		//update timer
		if (theNetSession->m_sessionSendLatencyInMilliseconds > connection->m_connectionSendLatencyInMilliseconds)
		{
			connection->m_latencySendTimer->SetTimerInMilliseconds(theNetSession->m_sessionSendLatencyInMilliseconds);
		}
		else
		{
			connection->m_latencySendTimer->SetTimerInMilliseconds(connection->m_connectionSendLatencyInMilliseconds);
		}
	}

	connection = nullptr;
	theNetSession = nullptr;
}

//  =========================================================================================
void SetGlobalHeartRate(Command& cmd)
{
	int heartbeatHzRate = cmd.GetNextInt();
	
	if(heartbeatHzRate < 0)
		heartbeatHzRate = 0;

	NetSession::GetInstance()->SetHeartbeatRate((float)heartbeatHzRate);

	DevConsolePrintf("Set global heart rate to %i hz", heartbeatHzRate);
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

//  =========================================================================================
bool OnHeartbeat(NetMessage & message, NetConnection * fromConnection)
{
	return true;
}




