#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Math\MathUtils.hpp"

//  =============================================================================
NetConnection::NetConnection()
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
		

	//setup heartbeat timer
	m_heartbeatTimer = new Stopwatch(GetMasterClock());
	m_heartbeatTimer->SetTimer(0.05f);
}

//  =============================================================================
NetConnection::~NetConnection()
{
	delete(m_latencySendTimer);
	m_latencySendTimer = nullptr;

	delete(m_heartbeatTimer);
	m_heartbeatTimer = nullptr;
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
	if(!m_latencySendTimer->ResetAndDecrementIfElapsed())
		return;

	NetSession* theNetSession = NetSession::GetInstance();

	//setup header
	NetPacketHeader header;
	header.m_senderIndex = theNetSession->m_sessionConnectionIndex;
	header.m_ack = GetNextAckToSend();
	header.m_highestReceivedAck = m_highestReceivedAck;
	header.m_receivedAckHistoryBitfield = m_receivedAckHistoryBitfield;

	NetPacket* packet = new NetPacket(header);
	PacketTracker* packetTracker = new PacketTracker();
	int reliableCount = 0;

	packet->WriteUpdatedPacketHeaderData();

	bool doesPacketHaveRoom = true;

	size_t totalMessageSize = 0;

	// sent reliables ----------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++messageIndex)
	{
		if (doesPacketHaveRoom)
		{
			//total buffer size + payload size + reliable header size + total message size (header + payload sizes)
			size_t totalMessageSize = packet->GetBufferSize() + m_unconfirmedSentReliablesMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalMessageSize <= PACKET_MTU)
			{
				//write message to packet
				packet->WriteMessage(*m_unconfirmedSentReliablesMessages[messageIndex], m_unconfirmedSentReliablesMessages[messageIndex]->m_header->m_reliableId);
				packetTracker->m_sentReliables[reliableCount] = m_unconfirmedSentReliablesMessages[messageIndex]->m_header->m_reliableId;
				++reliableCount;

				m_unconfirmedSentReliablesMessages[messageIndex]->m_sendTime = GetMasterClock()->GetLastHPC();			
			}
		}		
	}

	// unsent reliables ----------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_unsentReliableMessages.size(); ++messageIndex)
	{
		//check if CAN send reliable
		/*if (!CanSendNewReliableMessage())
		{
			break;
		}*/

		if (doesPacketHaveRoom)
		{
			//total buffer size + payload size + reliable header size + total message size (header + payload sizes)
			size_t totalMessageSize = packet->GetBufferSize() + m_unsentReliableMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalMessageSize <= PACKET_MTU)
			{
				//write message to packet
				packet->WriteMessage(*m_unsentReliableMessages[messageIndex], m_nextSentReliableId);
				packetTracker->m_sentReliables[reliableCount] = m_nextSentReliableId;
				++reliableCount;
				OnReliableSend();

				m_unsentReliableMessages[messageIndex]->m_sendTime = GetMasterClock()->GetLastHPC();
				m_unconfirmedSentReliablesMessages.push_back(m_unsentReliableMessages[messageIndex]);

				//cleanup
				m_unsentUnreliableMessages[messageIndex] = nullptr;
				m_unsentUnreliableMessages.erase(m_unsentUnreliableMessages.begin() + messageIndex);
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
			size_t totalMessageSize = packet->GetBufferSize() + m_unsentUnreliableMessages[messageIndex]->GetWrittenByteCount() + sizeof(uint8_t) + sizeof(uint16_t);

			//if we have room write to the current packet
			if (totalMessageSize <= PACKET_MTU)
			{
				//write message to packet
				packet->WriteMessage(*m_unsentUnreliableMessages[messageIndex]);
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
	SendPacket(packetTracker, packet);
}

//  =========================================================================================
void NetConnection::SendPacket(PacketTracker* packetTracker, NetPacket* packet)
{
	if (packet == nullptr)
		return;

	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->m_socket->SendTo(*m_address, packet->GetBuffer(), packet->GetWrittenByteCount());
	OnPacketSend(packetTracker, packet);
	m_sentPackets.push_back(packet);	

	theNetSession = nullptr;
}

//  =========================================================================================
void NetConnection::GetMessagesToResend(std::vector<NetMessage*>& outMessages)
{
	for (int messageIndex = 0; messageIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++messageIndex)
	{
		if (GetMasterClock()->GetLastHPC() - m_unconfirmedSentReliablesMessages[messageIndex]->m_sendTime > SecondsToPerformanceCounter(m_connectionResendRateInMilliseconds / 1000.0f))
		{
			outMessages.push_back(m_unconfirmedSentReliablesMessages[messageIndex]);
			m_unconfirmedSentReliablesMessages.erase(m_unconfirmedSentReliablesMessages.begin() + messageIndex);
			--messageIndex;
		}
	}
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

	if (m_trackedPackets[index].m_ack == ack)
	{
		uint64_t rttInHPC = GetMasterClock()->GetLastHPC() - m_trackedPackets[index].m_sendTime;

		//take blend of the two
		if (m_rttInMilliseconds > 0.f)
		{
			m_rttInMilliseconds = (m_rttInMilliseconds * 0.9f) + (PerformanceCounterToMilliseconds(rttInHPC) * 0.1f);
		}			
		else
		{
			m_rttInMilliseconds = PerformanceCounterToMilliseconds(rttInHPC);
		}
		

		m_myLastReceivedTimeInHPC = GetMasterClock()->GetLastHPC();
		m_trackedPackets[index].m_ack = UINT16_MAX;

		for (int messageIndex = 0; messageIndex < MAX_RELIABLES_PER_PACKET; ++messageIndex)
		{
			uint16_t reliableId = m_trackedPackets[index].m_sentReliables[messageIndex];
			for (int sentReliableMessageIndex = 0; sentReliableMessageIndex < (int)m_unconfirmedSentReliablesMessages.size(); ++sentReliableMessageIndex)
			{
				//if we got a reliable back remove it from the sent reliables list so we can stop resending
				if (m_unconfirmedSentReliablesMessages[sentReliableMessageIndex]->m_header->m_reliableId == reliableId)
				{
					m_unconfirmedSentReliablesMessages.erase(m_unconfirmedSentReliablesMessages.begin() + sentReliableMessageIndex);
					--sentReliableMessageIndex;
				}
			}
		}		
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

	if (m_trackedPackets[index].m_ack != INVALID_PACKET_ACK)
	{
		++m_numLostPackets;
		m_numLostPackets = ClampInt(m_numLostPackets, 0, MAX_TRACKED_PACKETS);
	}

	if (ack % MAX_TRACKED_PACKETS == 0)
	{
		m_lossPercentage = UpdateLossPercentage();
		m_numLostPackets = 0;
	}

	m_trackedPackets[index] = *tracker;
	
	delete(tracker);
	tracker = nullptr;
}

//  =============================================================================
float NetConnection::GetRoundTripTimeInSeconds()
{
	return 0.0f;
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
uint16_t NetConnection::GetLowestReliableId()
{
	return uint16_t();
}

//  =============================================================================
bool NetConnection::IsReliableConfirmed(uint16_t id) const
{
	return false;
}

//  =============================================================================
void NetConnection::MarkReliableReceived(uint16_t id)
{	
	if(CyclicGreaterThan(id, m_highestReceivedReliableId))
	{
	//if greater than the current highest, set as highest...
	//and put it in the list
	//and update the window (move the window and clear the list)
		m_highestReceivedReliableId = id;

		m_receivedReliableIds.push_back(id);

		uint16_t minWindowValue = m_highestReceivedReliableId - RELIABLE_WINDOW;
		for (int idIndex = 0; idIndex < (int)m_receivedReliableIds.size(); ++idIndex)
		{

		}
	
	}
	else
	{
		//else, less or equal to the highest...so add
		//m_receivedReliableIds.add(id);
	}
}

//  =============================================================================
bool NetConnection::ConfirmReliableId(uint16_t id)
{
	return false;
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
	//return m_receivedReliableIds.contains(id);

	return true;
}

