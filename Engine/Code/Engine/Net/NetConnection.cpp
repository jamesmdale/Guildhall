#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Math\MathUtils.hpp"

//  =============================================================================
NetConnection::NetConnection()
{
	Initialize();
}

//  =============================================================================
NetConnection::NetConnection(const NetAddress& address, const char* uniqueId, uint8_t connectionIndex)
{
	SetNetAddress(address);
	SetUniqueId(uniqueId);
	SetConnectionIndex(connectionIndex);

	Initialize();
}

//  =============================================================================
NetConnection::NetConnection(const NetConnectionInfo& info)
{
	m_info = info;
	Initialize();
}

//  =============================================================================
NetConnection::~NetConnection()
{
	delete(m_latencySendTimer);
	m_latencySendTimer = nullptr;

	delete(m_heartbeatTimer);
	m_heartbeatTimer = nullptr;

	//cleanup channels
	for (int channelIndex = 0; channelIndex < MAX_MESSAGE_CHANNELS; ++channelIndex)
	{
		if (m_netMessageChannels[channelIndex] != nullptr)
		{
			CloseMessageChannel((int)channelIndex);
		}
	}

	//delete unsent unreliable messages lingering
	for (int messageIndex = 0; messageIndex < (int)m_unsentUnreliableMessages.size(); ++messageIndex)
	{
		m_unsentUnreliableMessages.erase(m_unsentUnreliableMessages.begin() + messageIndex);
		--messageIndex;
	}

	//delete unsent reliable messages lingering
	for (int messageIndex = 0; messageIndex < (int)m_unsentReliableMessages.size(); ++messageIndex)
	{
		m_unsentReliableMessages.erase(m_unsentReliableMessages.begin() + messageIndex);
		--messageIndex;
	}

	//delete unsent unconfirmed reliable messages lingering
	for (int messageIndex = 0; messageIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++messageIndex)
	{
		m_unconfirmedSentReliablesMessages.erase(m_unconfirmedSentReliablesMessages.begin() + messageIndex);
		--messageIndex;
	}

	//sent packet cleanup
	for (int packetIndex = 0; packetIndex < (int)m_sentPackets.size(); ++packetIndex)
	{
		m_sentPackets.erase(m_sentPackets.begin() + packetIndex);
		--packetIndex;
	}

	//cleanup packet trackeres
	for (int packetIndex = 0; packetIndex < MAX_TRACKED_PACKETS; ++packetIndex)
	{
		delete(m_trackedPackets[packetIndex]);
		m_trackedPackets[packetIndex] = nullptr;
	}

	int i = 0;
}

//  =============================================================================
void NetConnection::Initialize()
{
	//setup simulated latency timer
	m_connectionSendLatencyInMilliseconds = 0.f;
	m_latencySendTimer = new Stopwatch(GetMasterClock());

	NetSession* theNetSession = NetSession::GetInstance();

	if (theNetSession->m_sessionSendLatencyInMilliseconds > m_connectionSendLatencyInMilliseconds)
	{
		m_latencySendTimer->SetTimerInMilliseconds(theNetSession->m_sessionSendLatencyInMilliseconds);
	}
	else
	{
		m_latencySendTimer->SetTimerInMilliseconds(m_connectionSendLatencyInMilliseconds);
	}

	for (int trackerIndex = 0; trackerIndex < MAX_TRACKED_PACKETS; ++trackerIndex)
	{
		m_trackedPackets[trackerIndex] = nullptr;
	}

	for(int channelIndex = 0; channelIndex < MAX_MESSAGE_CHANNELS; ++channelIndex)
	{
		OpenNewMessageChannel();
	}

	//setup heartbeat timer
	m_heartbeatTimer = new Stopwatch(GetMasterClock());
	m_heartbeatTimer->SetTimer(HEARTBEAT_TIMER);

	m_unsentUnreliableMessages.clear();
	m_unsentReliableMessages.clear();
	m_unconfirmedSentReliablesMessages.clear();

	m_sentPackets.clear();
}

//  =============================================================================
void NetConnection::QueueMessage(NetMessage* message)
{
	if (message->m_definition->IsReliable())
	{
		m_unsentReliableMessages.push_back(message);
	}
	else
	{
		m_unsentUnreliableMessages.push_back(message);
	}	
}

//  =============================================================================
void NetConnection::FlushOutgoingMessages()
{
	//early outs
	if(!m_latencySendTimer->ResetAndDecrementIfElapsed() || !DoesHaveMessagesToSend())
		return;


	int totalMessagesQueued  = GetTotalMessagesQueued();

	NetSession* theNetSession = NetSession::GetInstance();

	//setup header
	NetPacketHeader header;
	header.m_senderIndex = theNetSession->m_myConnection->GetConnectionIndex();
	header.m_ack = GetNextAckToSend();
	header.m_highestReceivedAck = m_highestReceivedAck;
	header.m_receivedAckHistoryBitfield = m_receivedAckHistoryBitfield;

	NetPacket* packet = new NetPacket(header);
	PacketTracker* packetTracker = new PacketTracker();

	int reliableCount = 0;

	packet->WriteUpdatedPacketHeaderData();

	bool doesPacketHaveRoom = true;

	size_t totalPacketSize = 0;

	// sent and unconfirmed reliables ----------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++messageIndex)
	{
		if (doesPacketHaveRoom)
		{
			//total buffer size + payload size + reliable header size + total message size (header + payload sizes)
			totalPacketSize = packet->GetBufferSize() + m_unconfirmedSentReliablesMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalPacketSize <= PACKET_MTU)
			{
				if (m_unconfirmedSentReliablesMessages[messageIndex]->IsReadyToResend(GetResendThresholdInHPC()))
				{
					//write message to packet
					packet->WriteMessage(*m_unconfirmedSentReliablesMessages[messageIndex], this, m_unconfirmedSentReliablesMessages[messageIndex]->m_header->m_reliableId);
					packetTracker->m_sentReliables[reliableCount] = m_unconfirmedSentReliablesMessages[messageIndex]->m_header->m_reliableId;
					++reliableCount;

					//reset age
					m_unconfirmedSentReliablesMessages[messageIndex]->m_sendTime = GetMasterClock()->GetLastHPC();	
				}						
			}
			else
			{
				doesPacketHaveRoom = false;
			}
		}		
	}

	// unsent reliables ----------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_unsentReliableMessages.size(); ++messageIndex)
	{
		//check if CAN send reliable
		if (!CanSendNewReliableMessage())
		{
			break;
		}

		if (doesPacketHaveRoom)
		{
			//total buffer size + payload size + connection in header size + reliable id in header size + total message size (header + payload sizes)
			totalPacketSize = packet->GetBufferSize() + m_unsentReliableMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalPacketSize <= PACKET_MTU)
			{
				//write message to packet
				packet->WriteMessage(*m_unsentReliableMessages[messageIndex], this, m_nextSentReliableId);
				packetTracker->m_sentReliables[reliableCount] = m_nextSentReliableId;
				++reliableCount;
				OnReliableSend();

				//reset age
				m_unsentReliableMessages[messageIndex]->m_sendTime = GetMasterClock()->GetLastHPC();
				m_unconfirmedSentReliablesMessages.push_back(m_unsentReliableMessages[messageIndex]);

				//cleanup
				m_unsentReliableMessages[messageIndex] = nullptr;
				m_unsentReliableMessages.erase(m_unsentReliableMessages.begin() + messageIndex);
				--messageIndex;
			}
			else
			{
				doesPacketHaveRoom = false;
			}
		}		
	}

	// unsent unreliables ----------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_unsentUnreliableMessages.size(); ++messageIndex)
	{
		if (doesPacketHaveRoom)
		{
			//total buffer size + payload size + unreliable header size + total message size (header + payload sizes)
			totalPacketSize = packet->GetBufferSize() + m_unsentUnreliableMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalPacketSize <= PACKET_MTU)
			{
				//write message to packet
				packet->WriteMessage(*m_unsentUnreliableMessages[messageIndex], this);
				m_unsentUnreliableMessages.erase(m_unsentUnreliableMessages.begin() + messageIndex);
				--messageIndex;
			}
			else
			{
				doesPacketHaveRoom = false;
			}
		}		
	}

	//clear unsent unreliables OTHERWISE, we will get any we missed on the next pass
	m_unsentUnreliableMessages.clear();

	if (totalMessagesQueued == 0 && m_forceSend)
	{
		packet->m_packetHeader.m_ack = INVALID_PACKET_ACK;
		packet->WriteUpdatedPacketHeaderData();
		packetTracker = nullptr;
	}

	SendPacket(packetTracker, packet);

	theNetSession = nullptr;

	//reset force send flag
	m_forceSend = false;
}

//  =========================================================================================
void NetConnection::SendPacket(PacketTracker* packetTracker, NetPacket* packet)
{
	if (packet == nullptr)
		return;

	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->m_socket->SendTo(GetNetAddress(), packet->GetBuffer(), packet->GetWrittenByteCount());
	OnPacketSend(packetTracker, packet);
	m_sentPackets.push_back(packet);	

	theNetSession = nullptr;
}

//  =============================================================================
bool NetConnection::DoesHaveMessagesToSend()
{
	//if we have any messages queued return true;
	return m_unconfirmedSentReliablesMessages.size() > 0 ||
		m_unsentReliableMessages.size() > 0 ||
		m_unsentUnreliableMessages.size() > 0 || m_forceSend == true;
}

//  =============================================================================
int NetConnection::GetTotalMessagesQueued()
{
	int messageCount = (int) (m_unconfirmedSentReliablesMessages.size() + m_unsentReliableMessages.size() + m_unsentUnreliableMessages.size());
	return messageCount;
}

//  =========================================================================================
void NetConnection::OnPacketSend(PacketTracker* packetTracker, NetPacket* packet)
{
	++m_nextSentAck;
	if (m_nextSentAck == INVALID_PACKET_ACK)
	{
		++m_nextSentAck;
	}

	m_lastSendTimeInHPC = GetMasterClock()->GetLastHPC();

	if(packetTracker != nullptr)
		AddPacketTracker(packetTracker, packet->GetAck());
}

//  =========================================================================================
void NetConnection::OnReliableSend()
{
	++m_nextSentReliableId;
}

//  =========================================================================================
void NetConnection::OnReceivePacket(NetPacket* packet)
{
	NetPacketHeader header = packet->m_packetHeader;

	//process the ack they sent me
	m_myLastReceivedTimeInHPC = GetMasterClock()->GetLastHPC();

	//update state based on what they say their highest received and bitfield history is
	OnMyAckReceived(header.m_highestReceivedAck);

	//mark received acks
	for (int bitfieldIndex = 0; bitfieldIndex < 16; ++bitfieldIndex)
	{
		uint bitFlag = 1 << bitfieldIndex;

		if (header.m_receivedAckHistoryBitfield & bitFlag)
		{
			OnMyAckReceived(header.m_highestReceivedAck - (bitfieldIndex + 1U));
		}
	}

	//update bitfield
	if (header.m_ack != INVALID_PACKET_ACK)
	{
		uint16_t distance = header.m_ack - m_highestReceivedAck;
		if (distance == 0)
		{
			return;  //this should never happen as acks increment in order
		}

		if (distance < (0xffff / 2))
		{
			m_highestReceivedAck = header.m_ack;

			//update bitfield
			m_receivedAckHistoryBitfield = m_receivedAckHistoryBitfield << distance;	//how many should I skip?
			m_receivedAckHistoryBitfield |= (1 << (distance - 1)); //set the old highest's bit
		} 
		else
		{
			//we got one older than the highest. We need to confirm that bit is set
			distance = m_highestReceivedAck - header.m_ack; //distance from highest
			m_receivedAckHistoryBitfield |= (1 << (distance - 1));
		}
	}	
}

//  =========================================================================================
void NetConnection::OnMyAckReceived(uint16_t ack)
{
	//if ack is invalid, just throw it out
	if (ack == UINT16_MAX)
	{
		return;
	}		

	int index = ack % MAX_TRACKED_PACKETS;

	if(m_trackedPackets[index] == nullptr)
		return;

	if (m_trackedPackets[index]->m_ack == ack)
	{
		uint64_t rttInHPC = GetMasterClock()->GetLastHPC() - m_trackedPackets[index]->m_sendTime;

		//take blend of the two
		if (rttInHPC > 0.f)
		{
			m_rttInSeconds = (m_rttInSeconds * 0.9f) + (PerformanceCounterToSeconds(rttInHPC) * 0.1f);
		}			
		else
		{
			m_rttInSeconds = PerformanceCounterToMilliseconds(rttInHPC);
		}
		
		m_myLastReceivedTimeInHPC = GetMasterClock()->GetLastHPC();

		for (int messageIndex = 0; messageIndex < MAX_RELIABLES_PER_PACKET; ++messageIndex)
		{
			uint16_t reliableId = m_trackedPackets[index]->m_sentReliables[messageIndex];

			if (reliableId != UINT16_MAX)
			{
				ConfirmSentReliable(reliableId);
			}			
		}

		delete m_trackedPackets[index];
		m_trackedPackets[index] = nullptr;
	}	
}

//  =============================================================================
uint16_t NetConnection::GetNextAckToSend()
{
	return m_nextSentAck;
}

//  =============================================================================
void NetConnection::AddPacketTracker(PacketTracker* tracker, uint16_t ack)
{
	tracker->m_ack = ack;
	tracker->m_sendTime = GetMasterClock()->GetLastHPC();

	int index = ack % MAX_TRACKED_PACKETS;

	if (m_trackedPackets[index] != nullptr)
	{
		++m_numLostPackets;
		m_numLostPackets = ClampInt(m_numLostPackets, 0, MAX_TRACKED_PACKETS);
	}

	if (ack % MAX_TRACKED_PACKETS == 0)
	{
		m_lossPercentage = UpdateLossPercentage();
		m_numLostPackets = 0;
	}

	m_trackedPackets[index] = tracker;
}

//  =============================================================================
float NetConnection::GetRoundTripTimeInSeconds()
{
	return m_rttInSeconds;
}

//  =============================================================================
float NetConnection::GetRoundTripTimeMilliseconds()
{
	return m_rttInSeconds * 1000.f;
}

//  =============================================================================
float NetConnection::UpdateLossPercentage()
{
	return ((float)m_numLostPackets / (float)MAX_TRACKED_PACKETS) * 100.f;
}

//  =============================================================================
float NetConnection::GetLossPercentage()
{
	return m_lossPercentage;
}

//  =============================================================================
float NetConnection::GetLastReceivedTimeInSeconds()
{
	uint64_t time = GetMasterClock()->GetLastHPC() - m_myLastReceivedTimeInHPC;
	return PerformanceCounterToSeconds(time);
}

//  =============================================================================
float NetConnection::GetLastSentTimeInSeconds()
{
	uint64_t time = GetMasterClock()->GetLastHPC() - m_lastSendTimeInHPC;
	return PerformanceCounterToSeconds(time);
}

//  =============================================================================
int NetConnection::GetLastSentAck()
{
	return m_nextSentAck - 1;
}

//  =============================================================================
int NetConnection::GetLastReceivedAck()
{
	return m_highestReceivedAck;
}

//  =============================================================================
bool NetConnection::IsMe() const
{
	return NetSession::GetInstance()->m_myConnection == (this) ? true : false;
}

//  =============================================================================
bool NetConnection::IsHost() const
{
	return NetSession::GetInstance()->m_hostConnection == (this) ? true : false;
}

//  =============================================================================
bool NetConnection::IsClient() const
{
	return NetSession::GetInstance()->m_myConnection != (this) ? true : false;
}

//  =============================================================================
bool NetConnection::IsTimedOut()
{
	//hasn't been started yet
	if(m_myLastReceivedTimeInHPC == 0)
		return false;

	if (GetLastReceivedTimeInSeconds() >= DEFAULT_CONNECTION_TIMEOUT_IN_SECONDS)
	{
		return true;
	}

	return false;
}

//  =============================================================================
void NetConnection::SetState(eNetConnectionState state)
{
	//if our state is already the set state, we don't need to send another message.
	if(m_state == state)
		return;

	m_state = state;

	if (IsMe())// && !IsHost())
	{
		NetMessage* message = new NetMessage("update_state");
		eNetConnectionState state = GetState();

		message->WriteBytes(sizeof(eNetConnectionState), (void*)&state, false);

		//send updated state to host connection
		NetSession* theNetSession = NetSession::GetInstance();
		if(theNetSession->m_boundConnections[0] != nullptr)
			theNetSession->m_boundConnections[0]->QueueMessage(message);
	}	
}

//  =============================================================================
void NetConnection::MarkReliableReceived(uint16_t id)
{	
	if(CyclicGreaterThan(id, m_highestReceivedReliableId))
	{
		//if greater than the current highest, set as highest...	
		m_highestReceivedReliableId = id;

		//and put it in the list
		m_receivedReliableIds.push_back(id);

		//and update the window (move the window and clear the list)
		uint16_t minWindowValue = m_highestReceivedReliableId - RELIABLE_WINDOW;
		for (int idIndex = 0; idIndex < (int)m_receivedReliableIds.size(); ++idIndex)
		{
			if (CyclicLessThan(m_receivedReliableIds[idIndex], minWindowValue))
			{
				m_receivedReliableIds.erase(m_receivedReliableIds.begin() + idIndex);
				--idIndex;
			}
		}
	}
	else
	{
		//else, less or equal to the highest...so add
		m_receivedReliableIds.push_back(id);
	}
}

//  =============================================================================
void NetConnection::ConfirmSentReliable(uint16_t id)
{
	for (int reliableIndex = 0; reliableIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++reliableIndex)
	{
		NetMessage* message = m_unconfirmedSentReliablesMessages[reliableIndex];
		if (message->m_header->m_reliableId == id)
		{
			//we found the id. Cleanup unconfirmed reliable messages list
			m_unconfirmedSentReliablesMessages.erase(m_unconfirmedSentReliablesMessages.begin() + reliableIndex);	
			return;
		}
		message = nullptr;
	}
}

//  =============================================================================
bool NetConnection::HasReceivedReliableId(uint16_t id)
{
	uint16_t maxWindowValue = m_highestReceivedReliableId;
	uint16_t minWindowValue = maxWindowValue - RELIABLE_WINDOW;

	// first, before the window?  (greater than window
	if (CyclicLessThan(id, minWindowValue))
	{
		return true; //we received it
	}

	//second, check if in list
	for (int reliableIndex = 0; reliableIndex < (int)m_receivedReliableIds.size(); ++reliableIndex)
	{
		if (m_receivedReliableIds[reliableIndex] == id)
		{
			return true;
		}
	}

	//if we didn't find it in the list or if it wasn't before the window..
	return false;
}

//  =============================================================================
bool NetConnection::CanSendNewReliableMessage()
{
	uint16_t nextReliableId = m_nextSentReliableId;

	if(m_unconfirmedSentReliablesMessages.size() == 0)
		return true;

	uint16_t oldestUnconfirmedReliableId = m_unconfirmedSentReliablesMessages[0]->m_header->m_reliableId;

	int diff = (int)nextReliableId - (int)oldestUnconfirmedReliableId;
	if (diff < (int)RELIABLE_WINDOW)
	{
		return true;
	}
	
	return false;
	
}

//  =============================================================================
uint64_t NetConnection::GetResendThresholdInHPC()
{
	return SecondsToPerformanceCounter(NET_RELIABLE_RESEND_RATE_PER_SECOND);
}

//  =============================================================================
uint8_t NetConnection::OpenNewMessageChannel()
{
	for (int channelIndex = 0; channelIndex < MAX_MESSAGE_CHANNELS; ++channelIndex)
	{
		if (m_netMessageChannels[channelIndex] == nullptr)
		{
			NetMessageChannel* channel = new NetMessageChannel();
			m_netMessageChannels[channelIndex] = channel;
			return channelIndex;
		}
	}

	//no channel available
	return UINT8_MAX;
}

//  =============================================================================
void NetConnection::CloseMessageChannel(uint8_t channelIndex)
{
	if (channelIndex < MAX_MESSAGE_CHANNELS)
	{
		delete(m_netMessageChannels[channelIndex]);
		m_netMessageChannels[channelIndex] = nullptr;
	}	
}

//  =============================================================================
uint16_t NetConnection::GetAndIncrementNextSequenceIdForChannel(uint8_t channelIndex)
{
	if (channelIndex < MAX_MESSAGE_CHANNELS)
	{
		uint16_t id = m_netMessageChannels[channelIndex]->GetAndIncrementSequenceId();	
		return id;
	}

	return UINT8_MAX;
}

