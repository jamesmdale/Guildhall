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
NetMessageDefinition* NetSession::s_registeredMessageDefinitions[UINT8_MAX];
Stopwatch* connectionRequestTimer = nullptr;
Stopwatch* timeoutTimer = nullptr; 

//  =============================================================================
NetSession::NetSession()
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		m_boundConnections[connectionIndex] = nullptr;
	}
}

//  =============================================================================
NetSession::~NetSession()
{
	//clear any remaining messages
	ProcessOutgoingMessages();

	//clear connections
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{		
		delete(m_boundConnections[connectionIndex]);
		m_boundConnections[connectionIndex] = nullptr;
	}

	delete(m_socket);
	m_socket = nullptr;
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
void NetSession::Startup()
{
	//register core messages
	RegisterCoreMessageTypes();

	//register other messages
	RegisterMessageDefinition("add", OnAdd);
	RegisterMessageDefinition("add_response", OnAddResponse);
	RegisterMessageDefinition("ack", OnAck);

	//register app commands

	//RegisterCommand("add_connection", CommandRegistration(AddConnectionToIndex, ": Attempt to add connection at index: idx IP", "Successfully added connection!"));
	RegisterCommand("host", CommandRegistration(SetToHost, ": Set this machine to host for game", ""));
	RegisterCommand("join", CommandRegistration(SetToJoin, ": Set this machine to join the given address", ""));
	RegisterCommand("disconnect", CommandRegistration(SetToDisconnect, ": Disconnect my connection from the session", ""));
	RegisterCommand("send_ping", CommandRegistration(SendPing, ": Send ping to index: idx", ""));
	RegisterCommand("send_add", CommandRegistration(SendAdd, ": Send add request to index: idx var1 var2", ""));
	RegisterCommand("send_multi_ping", CommandRegistration(SendMultiPing, ": Send two pings to index (tests multi message in single packet): idx", ""));
	RegisterCommand("net_sim_loss", CommandRegistration(SetNetSimLoss, ": Set the simulated loss: amount (value 0.0 to 1.0)", ""));
	RegisterCommand("net_sim", CommandRegistration(SetNetSimLag, ": Set the simulated network latency (in MS): min max ", ""));
	RegisterCommand("net_set_heart_rate", CommandRegistration(SetGlobalHeartRate, ": Set the heartbeat rate for all connections: rate", ""));
}

//  =========================================================================================
void NetSession::Shutdown()
{
	delete(g_theNetSession);
	g_theNetSession = nullptr;
}

//  =========================================================================================
void NetSession::Update(float deltaSeconds)
{
	//all registrations must be done BEFORE the first update
	if (!m_isDefinitionRegistrationLocked)
	{
		LockMessageDefinitionRegistration();
	}

	if(!m_myConnection->IsHost())
		UpdateNetClock(deltaSeconds);

	CheckHeartbeats();
	ProcessOutgoingMessages();

	if (m_socket != nullptr)
	{
		ProcessIncomingMessages();
		CheckDelayedPackets();	
	}

	//state management
	switch (m_state)
	{
	case SESSION_STATE_CONNECTING:
		UpdateConnecting();
		break;
	case SESSION_STATE_JOINING:
		UpdateJoining();
		break;
	case SESSION_STATE_READY:
		UpdateReady();
		break;
	}

	//cleanup
	CleanupConnections();
}

//  =============================================================================
void NetSession::UpdateConnecting()
{
	if (connectionRequestTimer == nullptr || timeoutTimer == nullptr)
	{
		connectionRequestTimer = new Stopwatch(0.1f, GetMasterClock());
		timeoutTimer = new Stopwatch(10.f, GetMasterClock());
	}

	if (connectionRequestTimer->ResetAndDecrementIfElapsed())
	{
		NetMessage* joinMessage = new NetMessage("join_request");
		m_hostConnection->QueueMessage(joinMessage);
	}
	
	//timeout timer
	if (timeoutTimer->ResetAndDecrementIfElapsed())
	{
		//cleanup timers
		ResetConnectionTimers();
		SetError(SESSION_ERROR_INTERNAL, Stringf("CONNECTION TIMEOUT TO ADDRESS: %s", m_hostConnection->GetNetAddress().ToString().c_str()).c_str());
		Disconnect();
	}
}

//  =============================================================================
void NetSession::UpdateJoining()
{
	ResetConnectionTimers();
}

//  =============================================================================
void NetSession::UpdateReady()
{
	ResetConnectionTimers();

	//foreach connection in connections where connection.isready...start doing stuff
}

//  =============================================================================
void NetSession::RegisterCoreMessageTypes()
{
	RegisterMessageDefinition(PING_CORE_NET_MESSAGE_TYPE, "ping", OnPing, CONNECTIONLESS_NET_MESSAGE_FLAG);
	RegisterMessageDefinition(PONG_CORE_NET_MESSAGE_TYPE, "pong", OnPong, CONNECTIONLESS_NET_MESSAGE_FLAG);
	RegisterMessageDefinition(HEARTBEAT_CORE_NET_MESSAGE_TYPE, "heartbeat", OnHeartbeat, HEARTBEAT_NET_MESSAGE_FLAG);

	RegisterMessageDefinition(JOIN_REQUEST_CORE_NET_MESSAGE_TYPE, "join_request", OnJoinRequest, CONNECTIONLESS_NET_MESSAGE_FLAG);
	RegisterMessageDefinition(JOIN_DENY_CORE_NET_MESSAGE_TYPE, "join_deny", OnJoinDenied, CONNECTIONLESS_NET_MESSAGE_FLAG);
	RegisterMessageDefinition(JOIN_ACCEPT_CORE_NET_MESSAGE_TYPE, "join_accept", OnJoinAccepted, RELIABLE_INORDER_NET_MESSAGE_FLAG, 0);
	RegisterMessageDefinition(JOIN_FINISHED_CORE_NET_MESSAGE_TYPE, "join_finished", OnJoinFinished, RELIABLE_INORDER_NET_MESSAGE_FLAG, 0);
	RegisterMessageDefinition(NEW_CONNECTION_CORE_NET_MESSAGE_TYPE, "new_connection", OnNewConnection, RELIABLE_INORDER_NET_MESSAGE_FLAG, 1);
	RegisterMessageDefinition(UPDATE_CONNECTION_STATE_CORE_NET_MESSAGE_TYPE, "update_state", OnUpdateConnectionState, RELIABLE_INORDER_NET_MESSAGE_FLAG, 2);
}

//  =============================================================================
void NetSession::ResetConnectionTimers()
{
	delete(connectionRequestTimer);
	connectionRequestTimer = nullptr;

	delete(timeoutTimer);
	timeoutTimer = nullptr;
}

//  =============================================================================
int NetSession::GetNumBoundConnections()
{
	int numBoundConnections = 0;
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if(m_boundConnections[connectionIndex] != nullptr)
			++numBoundConnections;
	}

	return numBoundConnections;
}

//  =============================================================================
int NetSession::GetFirstUnboundConnectionIndex()
{
	//skip 0 as it's meant for host
	for (int connectionIndex = 1; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if(m_boundConnections[connectionIndex] == nullptr)
			return connectionIndex;
	}

	return -1;
}

//  =============================================================================
void NetSession::Host(const char* myId, uint16_t port, uint16_t portRange)
{
	//early out. Must be in disconnected state
	if (m_state != SESSION_STATE_DISCONNECTED)
	{
		SetError(SESSION_ERROR_INTERNAL, "SESSION ALREADY CONNECTED");
		return;
	}		

	m_socket = new UDPSocket();
	if (!m_socket->BindToPort(port, portRange))
	{
		//if binding fails, update internal error
		SetError(SESSION_ERROR_INTERNAL, "PORT BINDING FAILURE");
		return;
	}

	NetConnectionInfo info;
	info.SetNetAddress(m_socket->GetAddress());
	info.SetUniqueId(myId);
	info.SetConnectionIndex(0);

	NetConnection* newHostConnection = CreateConnection(info);

	m_myConnection = newHostConnection;
	m_hostConnection = newHostConnection;

	//set connection as ready
	newHostConnection->SetState(CONNECTION_READY);
	m_state = SESSION_STATE_READY;
}

//  =============================================================================
void NetSession::Join(const char* myId, const NetConnectionInfo& hostInfo)
{
	//host info should be at connection 0
	m_hostConnection = CreateConnection(hostInfo);
	if (m_hostConnection == nullptr)
	{
		//do something
	}
	m_hostConnection->SetState(CONNECTION_CONNECTED);

	//create self
	m_socket = new UDPSocket();
	if (!m_socket->BindToPort(m_hostConnection->GetNetAddress().m_port, MAX_NUM_NET_CONNECTIONS))
	{
		//if binding fails, update internal error
		SetError(SESSION_ERROR_INTERNAL, "PORT BINDING FAILURE");
		return;
	}

	NetConnectionInfo info;
	info.SetNetAddress(m_socket->GetAddress());
	info.SetUniqueId(myId);
	info.SetConnectionIndex(INVALID_CONNECTION_INDEX);

	m_myConnection = CreateConnection(info);

	if (m_myConnection == nullptr)
	{
		//do something
	}

	//we don't care about the connection state. That's only for host. We only care about our own connection state
	m_state = SESSION_STATE_CONNECTING;	

	//send join request
}

//  =============================================================================
void NetSession::Disconnect()
{
	m_myConnection->SetState(CONNECTION_DISCONNECTED);
	
	m_myConnection->FlushOutgoingMessages();
}

//  =============================================================================
bool NetSession::IsDisconnected()
{
	return m_state == SESSION_STATE_DISCONNECTED ? true : false;
}

//  =============================================================================
bool NetSession::IsJoined()
{
	return m_state == SESSION_STATE_READY ? true : false;
}

//  =============================================================================
void NetSession::SetError(eNetSessionError error, const char* errorString)
{
	m_errorString = errorString;
	m_errorCode = error;
}

//  =============================================================================
void NetSession::ClearError()
{
	m_errorString = "";
	m_errorCode = SESSION_OK;
}

//  =============================================================================
eNetSessionError NetSession::GetLastError(std::string* outErrorString)
{
	outErrorString = &m_errorString;
	return m_errorCode;
}

//  =============================================================================
std::string NetSession::GenerateUniqueId()
{
	char value[MAX_UNIQUE_ID_LENGTH];

	uint64_t uniqueValue = GetMasterClock()->GetLastHPC();
	std::string uniqueString = std::to_string(uniqueValue);

	uint randomUint = GetRandomUintInRange(0, UINT_MAX - 1);

	uniqueValue = uniqueValue << randomUint;

	strcpy_s(value, MAX_UNIQUE_ID_LENGTH, uniqueString.c_str());

	return std::string(value);
}

//  =============================================================================
NetConnection* NetSession::CreateConnection(const NetConnectionInfo& info)
{
	NetConnection* connection = new NetConnection(info);

	m_allConnections.push_back(connection);

	if (GetNumBoundConnections() <= MAX_NUM_NET_CONNECTIONS && connection->GetConnectionIndex() != INVALID_CONNECTION_INDEX)
	{
		if (GetNumBoundConnections() > info.m_connectionIndex)
		{
			if (m_boundConnections[info.m_connectionIndex] != nullptr)
			{
				//connection index already taken.
				return connection;
			}
		}
		if (info.m_connectionIndex != INVALID_CONNECTION_INDEX)
		{
			BindConnection(info.m_connectionIndex, connection);	
		}			
	}

	return connection;
}

//  =============================================================================
void NetSession::DestroyConnection(NetConnection * connection)
{
	//clear conevenience pointers
	if (connection == m_myConnection)
	{
		m_myConnection = nullptr;
	}

	if (connection == m_hostConnection)
	{
		m_hostConnection = nullptr;
	}

	//remove from bound connections
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if(connection == m_boundConnections[connectionIndex])
			m_boundConnections[connectionIndex] = nullptr;
	}

	//delete
	for (int connectionIndex = 0; connectionIndex < (int)m_allConnections.size(); ++connectionIndex)
	{
		if (connection == m_allConnections[connectionIndex])
		{
			m_allConnections.erase(m_allConnections.begin() + connectionIndex);
			m_allConnections[connectionIndex] = nullptr;
		}			
	}
}

//  =============================================================================
void NetSession::BindConnection(uint8_t connectionIndex, NetConnection* connection)
{
	ASSERT_OR_DIE(connectionIndex < MAX_NUM_NET_CONNECTIONS, "INVALID CONNECTION INDEX ATTEMPTED IN BIND CONNECTION");

	m_boundConnections[connectionIndex] = connection;
}

//  =============================================================================
NetConnection * NetSession::GetBoundConnectionByAddress(const NetAddress & address)
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if (m_boundConnections[connectionIndex] != nullptr)
		{
			NetAddress connAddress = m_boundConnections[connectionIndex]->GetNetAddress();
			if (connAddress == address)
			{
				return m_boundConnections[connectionIndex];
			}
		}		
	}

	return nullptr;
}

//  =============================================================================
int NetSession::GetConnectionIndexFromAllConnectionsByAddress(const NetAddress& address)
{
	for (int connectionIndex = 0; connectionIndex < (int)m_allConnections.size(); ++connectionIndex)
	{
		if (m_allConnections[connectionIndex] != nullptr)
		{
			NetAddress connAddress = m_allConnections[connectionIndex]->GetNetAddress();
			if (connAddress == address)
			{
				return connectionIndex;
			}
		}		
	}

	return -1;
}

//  =============================================================================
void NetSession::CleanupConnections()
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if(m_boundConnections[connectionIndex] == nullptr)
			continue;

		if (m_boundConnections[connectionIndex]->GetState() == CONNECTION_DISCONNECTED || m_boundConnections[connectionIndex]->IsTimedOut())
		{
			if (m_boundConnections[connectionIndex] == m_myConnection || m_boundConnections[connectionIndex] == m_hostConnection)
			{
				DisconnectNetSession();
				return;
			}
			else
			{		
				int foundConnectionIndex = GetConnectionIndexFromAllConnectionsByAddress(m_boundConnections[connectionIndex]->GetNetAddress());
				m_boundConnections[connectionIndex] = nullptr;

				ASSERT_OR_DIE(foundConnectionIndex != -1, "INVALID CONNECTION DURING CLEANUP: CONNECTION DOESN'T EXIST IN LIST");

				delete(m_allConnections[foundConnectionIndex]);
				m_allConnections[foundConnectionIndex] = nullptr;
			}
		}
	}
}

//  =========================================================================================
void NetSession::CheckHeartbeats()
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if (m_boundConnections[connectionIndex] != nullptr)
		{
			if (m_boundConnections[connectionIndex]->m_heartbeatTimer->ResetAndDecrementIfElapsed())
			{
				SendHeartBeat(m_boundConnections[connectionIndex]->GetConnectionIndex());
			}
		}
	}
}

//  =========================================================================================
void NetSession::SendHeartBeat(int index)
{
	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetBoundConnectionById((uint8_t)index);
	if (connection == nullptr)
	{
		DevConsolePrintf("No connection at index %u", index);
		return;
	}

	//only really used by the host. Client reads on other end
	uint netTime = GetNetTimeInMilliseconds();
	NetMessage* message = new NetMessage("heartbeat");
	bool success = message->WriteBytes(sizeof(uint), &netTime, false);

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
	//check packet validity
	if (!packet->m_packet->CheckIsValid())
	{
		return;
	}

	//get the header data
	packet->m_packet->ReadHeader(packet->m_packet->m_packetHeader);

	NetConnection* connection = GetBoundConnectionById(packet->m_packet->m_packetHeader.m_senderIndex);

	//we don't have a connection on this index (probably invalid)
	if ((packet->m_packet->m_packetHeader.m_senderIndex != UINT8_MAX) && (connection == nullptr))
	{
		return;
	}

	//handle on receive packet
	if (connection != nullptr)
	{
		connection->OnReceivePacket(packet->m_packet);
	}
	else
	{
		connection = new NetConnection();
		connection->SetNetAddress(packet->m_senderAddress);
	}

	for(int messageIndex = 0; messageIndex < (int)packet->m_packet->m_packetHeader.m_messageCount; ++messageIndex)
	{
		uint16_t totalSize = UINT16_MAX;
		uint8_t callbackId = UINT8_MAX;
		uint16_t reliableId = 0;

		packet->m_packet->ReadBytes(&totalSize, sizeof(uint16_t), false);
		packet->m_packet->ReadBytes(&callbackId, sizeof(uint8_t), false);
		
		NetMessageDefinition* definition = GetRegisteredDefinitionById((int)callbackId);

		if (definition != nullptr)
		{
			//if the definition requires a connection and the we don't have one for this sender, throw the message out
			if (definition->DoesRequireConnection() && connection->GetConnectionIndex() == UINT8_MAX)
			{
				DebuggerPrintf("MESSAGE RECEIVED WITHOUT CONNECTION");
				continue;
			}

			NetMessage* message = new NetMessage(definition->m_callbackName);

			//header callback id
			size_t headerSize = sizeof(uint8_t);

			//get reliable id and adjust remaining size 
			if (message->m_definition->IsReliable())
			{
				//we need to send a response next frame
				connection->m_forceSend = true;

				packet->m_packet->ReadBytes(&reliableId, sizeof(uint16_t), false);
				message->m_header->m_reliableId = reliableId;

				headerSize += sizeof(uint16_t);

				uint16_t reliableId = message->GetReliableId();
				if (!connection->HasReceivedReliableId(reliableId))
				{
					connection->MarkReliableReceived(reliableId);
				}
			}

			//if there is more to the message we must read that in before calling the return function
			size_t remainingAmountToRead = totalSize - headerSize;			
			if (remainingAmountToRead > 0)
			{
				message->ExtendBufferSize(remainingAmountToRead);
				packet->m_packet->ReadBytes(message->GetBuffer(), remainingAmountToRead, false);
				message->SetWrittenByteCountToBufferSize();
			}		

			//if the message requires the correct order, decide how to process the message

			if (message->m_definition->IsInOrder())
			{
				uint8_t channelIndex = message->m_definition->m_messageChannelIndex;
				message->ReadBytes(&message->m_header->m_sequenceId, sizeof(uint16_t), false);

				bool shouldProcessMessageInorderMessage = connection->m_netMessageChannels[channelIndex]->ShouldProcessMessage(message);				
				while (shouldProcessMessageInorderMessage)
				{
					ExecuteNetMessage(message, connection);

					//see if we have any other messages in our unordered queue and process it
					message = connection->m_netMessageChannels[channelIndex]->GetNextMessageToProcess();
					if (message == nullptr)
					{
						shouldProcessMessageInorderMessage = false;
					}
				}
			}
			else
			{
				ExecuteNetMessage(message, connection);
			}			
		}				

		definition = nullptr;
	}

	//cleanup connection if we don't actually have a valid one for this person
	if (connection->GetConnectionIndex() == INVALID_CONNECTION_INDEX)
	{
		delete(connection);
	}

	connection = nullptr;		
}

//  =============================================================================
void NetSession::ExecuteNetMessage(NetMessage* message, NetConnection* connection)
{
	message->m_definition->m_callback(*message, connection);

	//cleanup
	delete(message);
	message = nullptr;
}

//  =============================================================================
void NetSession::ProcessOutgoingMessages()
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if(m_boundConnections[connectionIndex] != nullptr)
		{
			m_boundConnections[connectionIndex]->FlushOutgoingMessages();
		}
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
	NetPacket* packet = new NetPacket(theNetSession->m_myConnection->GetConnectionIndex(), 0);
	packet->WriteUpdatedPacketHeaderData();

	if (packet->GetBufferSize() + message->GetWrittenByteCount() <= PACKET_MTU)
	{
		//write message to packet
		packet->WriteMessage(*message, nullptr);
		packetsToSend.push_back(packet);
	}

	//send each packet
	for(int packetIndex = 0; packetIndex < (int)packetsToSend.size(); ++packetIndex)
	{
		size_t amountSent = theNetSession->m_socket->SendTo(connection->GetNetAddress(), packetsToSend[packetIndex]->GetBuffer(), packetsToSend[packetIndex]->GetWrittenByteCount());
	
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

		bool isPositionValid = false;
		while (!isPositionValid)
		{
			//we are out of space
			if (m_nextRegisteredDefinitionIndex > MAX_NET_DEFINITION_REGISTRATIONS)
				return false;

			if (s_registeredMessageDefinitions[m_nextRegisteredDefinitionIndex] == nullptr)
			{
				s_registeredMessageDefinitions[m_nextRegisteredDefinitionIndex] = definition;
				isPositionValid = true;
				success = true;
			}	

			++m_nextRegisteredDefinitionIndex;
		}
	}

	return success;
}

//  =============================================================================
bool NetSession::RegisterMessageDefinition(int netMessageId, const std::string& name, NetMessageCallback callback, const eNetMessageFlag& flag, const uint8_t& messageChannelIndex)
{
	bool success = false;

	if (!m_isDefinitionRegistrationLocked)
	{
		NetMessageDefinition* definition = new NetMessageDefinition();
		definition->m_callbackName = name;
		definition->m_callback = callback;
		definition->m_callbackId = netMessageId;
		definition->m_messageFlag = flag;
		definition->m_messageChannelIndex = messageChannelIndex;

		if (s_registeredMessageDefinitions[netMessageId] == nullptr)
		{
			s_registeredMessageDefinitions[netMessageId] = definition;

			if (m_nextRegisteredDefinitionIndex == netMessageId)
			{
				++m_nextRegisteredDefinitionIndex;
			}
		}	
	}

	return success;
}

//  =========================================================================================
void NetSession::LockMessageDefinitionRegistration()
{
	m_isDefinitionRegistrationLocked = true;
	SortMessageDefinitionsByName();
	AssignFinalDefinitionIds();
}

//  =========================================================================================
void NetSession::SortMessageDefinitionsByName()
{
	//sort by name. excluding callbackids that are already assigned
	int i = 0;
	int j = 0;

	for (i = 0; i < (int)MAX_NET_DEFINITION_REGISTRATIONS - 1; ++i)
	{
		for (j = 0; j < (int)MAX_NET_DEFINITION_REGISTRATIONS - i - 1; ++j)
		{
			if (s_registeredMessageDefinitions[j] != nullptr && s_registeredMessageDefinitions[j + 1] != nullptr)
			{
				//check if one of these two definitions is fixed
				if (s_registeredMessageDefinitions[j]->m_callbackId < 0 && s_registeredMessageDefinitions[j + 1]->m_callbackId < 0)
				{
					if (CompareStringAlphabeticalLessThan(s_registeredMessageDefinitions[j]->m_callbackName, s_registeredMessageDefinitions[j + 1]->m_callbackName))
					{
						std::swap(s_registeredMessageDefinitions[j], s_registeredMessageDefinitions[j + 1]);
					}
				}
			}					
		}
	}
}

//  =========================================================================================
void NetSession::AssignFinalDefinitionIds()
{
	for (int definitionIndex = 0; definitionIndex < (int)MAX_NET_DEFINITION_REGISTRATIONS; ++definitionIndex)
	{
		if (s_registeredMessageDefinitions[definitionIndex] != nullptr)
		{
			//if the callback is still unassigned, make the callbackid equal the current index
			if (s_registeredMessageDefinitions[definitionIndex]->m_callbackId < 0)
			{
				s_registeredMessageDefinitions[definitionIndex]->m_callbackId = definitionIndex;
			}
		}			
	}
}

//  =========================================================================================
NetConnection* NetSession::GetBoundConnectionById(uint8_t id)
{
	if (id < MAX_NUM_NET_CONNECTIONS )
	{
		return m_boundConnections[(int)id];
	}

	return nullptr;	
}

//  =========================================================================================
void NetSession::SetHeartbeatRate(float hertz)
{
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if (m_boundConnections[connectionIndex] != nullptr)
		{
			if (hertz == 0)
			{
				m_boundConnections[connectionIndex]->m_heartbeatTimer->SetTimer(0);
			}
			else
			{
				m_boundConnections[connectionIndex]->m_heartbeatTimer->SetTimer(1.f / hertz);
			}
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

//  =============================================================================
void NetSession::DisconnectNetSession()
{
	bool isAlsoHost = m_myConnection->IsHost();

	//cleanup self and host
	delete(m_myConnection);
	m_myConnection = nullptr;

	if (!isAlsoHost)
	{
		delete(m_hostConnection);
	}
	m_hostConnection = nullptr;

	//cleanup any remaining connections
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		m_boundConnections[connectionIndex] = nullptr;
	}

	//call final delete across all connections
	for (int connectionIndex = 0; connectionIndex < (int)m_allConnections.size(); ++connectionIndex)
	{
		if (m_allConnections[connectionIndex] != nullptr)
		{
			m_allConnections.erase(m_allConnections.begin() + connectionIndex);
			--connectionIndex;
		}
	}

	//cleanup socket
	delete(m_socket);
	m_socket = nullptr;

	for (int packetIndex = 0; packetIndex < m_delayedPackets.size(); ++packetIndex)
	{
		m_delayedPackets.erase(m_delayedPackets.begin() + packetIndex);
		--packetIndex;
	}

	SetState(SESSION_STATE_DISCONNECTED);
}

//  =============================================================================
void NetSession::UpdateNetClock(float deltaSeconds)
{
	uint deltaMilliseconds = SecondsToMilliseconds(deltaSeconds);

	m_desiredClientTimeInMilliseconds += deltaMilliseconds;

	if (deltaMilliseconds + m_currentClientTimeInMilliseconds > m_desiredClientTimeInMilliseconds)
	{
		m_currentClientTimeInMilliseconds += (1.f - MAX_NET_TIME_DILATION) * deltaMilliseconds;
	}
	else if (deltaMilliseconds + m_currentClientTimeInMilliseconds < m_desiredClientTimeInMilliseconds)
	{
		m_currentClientTimeInMilliseconds += (1.f + MAX_NET_TIME_DILATION) * deltaMilliseconds;
	}
}

//  =============================================================================
uint NetSession::GetNetTimeInMilliseconds()
{
	if(m_myConnection == nullptr)
		return UINT_MAX;

	if(m_myConnection->IsHost())
		return (uint)GetMasterClock()->GetRunningTimeInMilliseconds();

	if(!m_myConnection->IsHost())
		return m_currentClientTimeInMilliseconds;
}

//  =============================================================================
void NetSession::UpdateClientNetClock(uint hostTime)
{
	static bool isFirstUpdate = true;

	if(hostTime < m_lastReceivedHostTimeInMilliseconds)
		return;

	//only happens on the first pass
	if (isFirstUpdate)
	{
		m_currentClientTimeInMilliseconds = hostTime;
		isFirstUpdate = false;
	}		

	m_lastReceivedHostTimeInMilliseconds = hostTime + (m_myConnection->GetRoundTripTimeMilliseconds() * 0.5f);
	m_desiredClientTimeInMilliseconds = hostTime;
}

//  =========================================================================================
//  Callback
//  =========================================================================================

NetMessageDefinition* GetRegisteredDefinitionById(int id)
{
	if (id < 0 || id > MAX_NET_DEFINITION_REGISTRATIONS)
	{
		return nullptr;
	}

	return NetSession::GetInstance()->s_registeredMessageDefinitions[id];
}

//  =========================================================================================
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string& name)
{
	//registered messages SHOULD be sorted by name unless they had a set value
	//for loop should be adequate most of the time

	NetSession* theNetSession = NetSession::GetInstance();

	for (int definitionIndex = 0; definitionIndex < MAX_NET_DEFINITION_REGISTRATIONS; ++definitionIndex)
	{
		if (theNetSession->s_registeredMessageDefinitions[definitionIndex] != nullptr)
		{
			if (theNetSession->s_registeredMessageDefinitions[definitionIndex]->m_callbackName.compare(name) == 0)
			{
				return theNetSession->s_registeredMessageDefinitions[definitionIndex];
			}
		}		
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
	//int index = cmd.GetNextInt();
	//std::string address = cmd.GetNextString();

	//NetSession* theNetSession = NetSession::GetInstance();

	//if (theNetSession->m_boundConnections.size() != index)
	//{
	//	DevConsolePrintf(Rgba::RED, "Requested index %i is invalid", index);
	//}

	//else if (IsStringNullOrEmpty(address))
	//{
	//	DevConsolePrintf(Rgba::RED, "Address is invalid");
	//}		
	//else
	//{
	//	NetAddress* netAddress = new NetAddress(address.c_str());
	//	if (netAddress == nullptr)
	//	{
	//		DevConsolePrintf(Rgba::RED, "Address is invalid");
	//	}
	//	else
	//	{
	//		NetSession::GetInstance()->AddConnection(index, netAddress);
	//		DevConsolePrintf("Successfully added address (%s) at index %i", address.c_str(), index);
	//	}	
	//	netAddress = nullptr;
	//}

	////cleanup
	//theNetSession = nullptr;
}

//  =============================================================================
void SetToHost(Command& cmd)
{
	NetSession* theNetSesssion = NetSession::GetInstance();

	std::string id = theNetSesssion->GenerateUniqueId();

	int port = cmd.GetNextInt();
	int portRange = cmd.GetNextInt();

	if(portRange == INT_MAX)
		portRange = 0;

	//cmd error checking
	if (port >= UINT16_MAX || port < 0)
	{
		DevConsolePrintf(Rgba::RED, "INVALID PORT (%i)!", port);
		return;
	}
	if (portRange >= UINT16_MAX || portRange < 0)
	{
		DevConsolePrintf(Rgba::RED, "INVALID PORT RANGE (%i)!", portRange);
		return;
	}

	//attempt to host with given parameters
	theNetSesssion->Host(id.c_str(), port, portRange);

	//check error code generated from Host function
	if (theNetSesssion->m_errorCode == SESSION_ERROR_INTERNAL)
	{
		DevConsolePrintf("SESSION ERROR: CODE(%i): %s", (int)theNetSesssion->m_errorCode, theNetSesssion->m_errorString.c_str());
	}
}

//  =============================================================================
void SetToDisconnect(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	if (theNetSession->m_myConnection == nullptr)
	{
		DevConsolePrintf(Rgba::RED, "INVALID DISCONNECT: Not currently connected to a host");	
	}

	theNetSession->Disconnect();
	DevConsolePrintf(Rgba::GREEN, "Successfully disconnected from host.");	

	//cleanup
	theNetSession = nullptr;
}

//  =============================================================================
void SetToJoin(Command & cmd)
{
	std::string address = cmd.GetNextString();

	NetSession* theNetSession = NetSession::GetInstance();

	if (IsStringNullOrEmpty(address))
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
			NetConnectionInfo hostInfo;
			hostInfo.m_connectionIndex = 0;
			hostInfo.m_address = *netAddress;

			theNetSession->Join("", hostInfo);
			DevConsolePrintf("Successfully added host address (%s)", address.c_str());
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

	NetConnection* connection = theNetSession->GetBoundConnectionById((uint8_t)index);
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

	NetConnection* connection = theNetSession->GetBoundConnectionById((uint8_t)index);
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
	
	NetConnection* connection = theNetSession->GetBoundConnectionById((uint8_t)index);
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
	for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
	{
		if (theNetSession->m_boundConnections[connectionIndex] != nullptr)
		{
			if (theNetSession->m_sessionSendLatencyInMilliseconds > theNetSession->m_boundConnections[connectionIndex]->m_connectionSendLatencyInMilliseconds)
			{
				theNetSession->m_boundConnections[connectionIndex]->m_latencySendTimer->SetTimerInMilliseconds(theNetSession->m_sessionSendLatencyInMilliseconds);
			}
		}		
	}
}

//  =========================================================================================
void SetConnectionSendRate(Command& cmd)
{
	int connectionIndex = cmd.GetNextInt();
	float sendRate = cmd.GetNextFloat();

	NetSession* theNetSession = NetSession::GetInstance();

	NetConnection* connection = theNetSession->GetBoundConnectionById((uint8_t)connectionIndex);
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
	DevConsolePrintf("Received PING from %s (Connection:%i)", fromConnection->GetNetAddress().ToString().c_str(), (int)fromConnection->GetConnectionIndex());

	NetMessage* pongMessage = new NetMessage("pong");

	// messages are sent to connections (not sessions)
	if (fromConnection->GetConnectionIndex() != UINT8_MAX)
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
	DevConsolePrintf("Received PONG from %s (Connection:%i)", fromConnection->GetNetAddress().ToString().c_str(), (int)fromConnection->GetConnectionIndex());
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
		DevConsolePrintf("Received ADD function from connection %i. Could not read param1", fromConnection->GetConnectionIndex() );
		return success;
	}

	success = message.ReadBytes(&parameter2, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param2", fromConnection->GetConnectionIndex() );
		return success;
	}

	float sum = parameter1 + parameter2;
	DevConsolePrintf("Received ADD function from connection %i: %f + %f = %f", fromConnection->GetConnectionIndex(),
	parameter1,
	parameter2,
	sum);

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
		if (fromConnection->GetConnectionIndex() != UINT8_MAX)
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
		DevConsolePrintf("Received ADD function from connection %i. Could not read param1", fromConnection->GetConnectionIndex());
		return success;
	}

	success = message.ReadBytes(&parameter2, sizeof(float), false);

	if (!success)
	{
		DevConsolePrintf("Received ADD function from connection %i. Could not read param2", fromConnection->GetConnectionIndex());
		return success;
	}

	success = message.ReadBytes(&sum, sizeof(float), false);

	DevConsolePrintf("Received ADD response from connection %i: %f + %f = %f", fromConnection->GetConnectionIndex(),
		parameter1,
		parameter2,
		sum);

	return true;
}

//  =========================================================================================
bool OnHeartbeat(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();

	//we need to read the hosts time that they passed
	if (!theNetSession->m_myConnection->IsHost() && !fromConnection->IsMe())
	{
		uint hostTime = UINT_MAX;
		message.ReadBytes(&hostTime, sizeof(uint), false);

		theNetSession->UpdateClientNetClock(hostTime);
	}

	return true;
}

//  =========================================================================================
bool OnAck(NetMessage& message, NetConnection* fromConnection)
{
	uint16_t ack = INVALID_PACKET_ACK;
	bool success = message.ReadBytes(&ack, sizeof(uint16_t), false);

	if (success)
	{
		fromConnection->OnMyAckReceived(ack);
	}	

	return success;
}

//  =============================================================================
bool OnJoinRequest(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();

	bool successfulConnection = false;
	if (theNetSession->m_myConnection != theNetSession->m_hostConnection ||
		theNetSession->GetNumBoundConnections() >= MAX_NUM_NET_CONNECTIONS)
	{
		NetMessage* failMessage = new NetMessage("join_deny");
		theNetSession->m_myConnection->QueueMessage(failMessage);
		return false;
	}
	
	//ignore the request if they are already connected
	else if(theNetSession->GetBoundConnectionByAddress(fromConnection->GetNetAddress()) != nullptr)
	{
		return false;
	}

	else
	{
		NetConnectionInfo info;
		info.SetNetAddress(fromConnection->GetNetAddress());
		info.SetUniqueId(theNetSession->GenerateUniqueId().c_str());
		info.SetConnectionIndex(theNetSession->GetFirstUnboundConnectionIndex());

		NetConnection* newConnection = theNetSession->CreateConnection(info);
		if (newConnection == nullptr)
		{
			return false;
		}		
		else
		{
			theNetSession->m_allConnections.push_back(newConnection);
			theNetSession->BindConnection(info.m_connectionIndex, newConnection);
			newConnection->SetState(CONNECTION_CONNECTING);

			NetMessage* successMessage = new NetMessage("join_accept");

			size_t val = sizeof(char[MAX_UNIQUE_ID_LENGTH]);

			successMessage->WriteBytes(sizeof(char[MAX_UNIQUE_ID_LENGTH]), &theNetSession->m_hostConnection->m_info.m_uniqueId, false);
			successMessage->WriteBytes(sizeof(char[MAX_UNIQUE_ID_LENGTH]), &info.m_uniqueId, false);
			successMessage->WriteBytes(sizeof(uint8_t), &info.m_connectionIndex, false);
			theNetSession->m_boundConnections[info.m_connectionIndex]->QueueMessage(successMessage);

			//used for P2P
			NetMessage* finishedMessage = new NetMessage("join_finished");
			theNetSession->m_boundConnections[info.m_connectionIndex]->QueueMessage(finishedMessage);
			return true;
		}
	}
}

//  =============================================================================
bool OnJoinDenied(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->Disconnect();

	return true;
}

//  =============================================================================
bool OnJoinAccepted(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();

	//connection already accepted
	if ((theNetSession->m_myConnection->GetState() == CONNECTION_CONNECTED || theNetSession->m_myConnection->GetState() == CONNECTION_READY))
	{
		return false;
	}

	char hostId[MAX_UNIQUE_ID_LENGTH];
	message.ReadBytes(&hostId, sizeof(char[MAX_UNIQUE_ID_LENGTH]), false);
	theNetSession->m_hostConnection->m_info.SetUniqueId(hostId);

	NetConnectionInfo myInfo;
	
	message.ReadBytes(&myInfo.m_uniqueId, sizeof(char[MAX_UNIQUE_ID_LENGTH]), false);
	message.ReadBytes(&myInfo.m_connectionIndex, sizeof(uint8_t), false);

	theNetSession->m_myConnection->m_info.m_connectionIndex = myInfo.m_connectionIndex;
	theNetSession->m_myConnection->m_info.SetUniqueId(myInfo.m_uniqueId);

	theNetSession->BindConnection(theNetSession->m_myConnection->m_info.m_connectionIndex, theNetSession->m_myConnection);

	//update state
	theNetSession->m_myConnection->SetState(CONNECTION_CONNECTED);
	theNetSession->SetState(SESSION_STATE_JOINING);

	return true;
}

//  =============================================================================
bool OnNewConnection(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =============================================================================
bool OnJoinFinished(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();

	//we are already finished joining.
	if (theNetSession->m_myConnection->GetState() == CONNECTION_READY)
	{
		return false;
	}
	
	theNetSession->m_myConnection->SetState(CONNECTION_READY);
	theNetSession->SetState(SESSION_STATE_READY);

	return true;
}

//  =============================================================================
bool OnUpdateConnectionState(NetMessage& message, NetConnection* fromConnection)
{
	NetSession* theNetSession = NetSession::GetInstance();	
	NetConnection* connection = theNetSession->GetBoundConnectionById(fromConnection->GetConnectionIndex());

	if(connection == nullptr)
		return false;

	eNetConnectionState newConnectionState;
	message.ReadBytes(&newConnectionState, sizeof(eNetConnectionState), false);

	connection->SetState(newConnectionState);

	return true;
}





