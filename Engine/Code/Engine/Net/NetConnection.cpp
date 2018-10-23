#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetPacket.hpp"

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
	m_outgoingUnreliableMessages.push_back(message);
}

//  =============================================================================
void NetConnection::FlushOutgoingMessages()
{
	if(m_outgoingUnreliableMessages.size() == 0)
		return;

	NetSession* theNetSession = NetSession::GetInstance();

	bool areMessagesPacked = false;
	int currentMessageIndex = 0;

	//setup header
	NetPacketHeader header;
	header.m_senderIndex = theNetSession->m_sessionConnectionIndex;
	header.m_ack = GetNextAckToSend();
	header.m_highestReceivedAck = m_highestReceivedAck;
	header.m_receivedAckHistoryBitfield = m_receivedAckHistoryBitfield;

	NetPacket* packet = new NetPacket(header);
	packet->WriteUpdatedHeaderData();
	
	while (!areMessagesPacked)
	{
		//total buffer size + payload size + header size + total message size (header + payload sizes)
		if (packet->GetBufferSize() + m_outgoingUnreliableMessages[currentMessageIndex]->GetWrittenByteCount() + sizeof(NetMessageHeader) + sizeof(uint16_t) <= PACKET_MTU)
		{
			//write message to packet
			packet->WriteMessage(*m_outgoingUnreliableMessages[currentMessageIndex]);
		}
		else
		{
			//in order to pack the next message, we need to make a new packet
			//first add old packet to list
			m_readyPackets.push_back(packet);

			//make a new packet
			//setup header
			NetPacketHeader header;
			header.m_senderIndex = theNetSession->m_sessionConnectionIndex;
			header.m_ack = GetNextAckToSend();
			header.m_highestReceivedAck = m_highestReceivedAck;
			header.m_receivedAckHistoryBitfield = m_receivedAckHistoryBitfield;

			NetPacket* packet = new NetPacket(header);
			packet->WriteUpdatedHeaderData();

			//write new message to new packet
			packet->WriteMessage(*m_outgoingUnreliableMessages[currentMessageIndex]);
		}

		//if we have read all the messages we are done
		if (currentMessageIndex == (int)m_outgoingUnreliableMessages.size() - 1)
		{
			areMessagesPacked = true;
			
			//add the last packet to the list to send
			m_readyPackets.push_back(packet);
		}
		else
		{
			currentMessageIndex++;		
		}
	}

	m_outgoingUnreliableMessages.clear();

	/*delete(packet);
	packet = nullptr;*/

	theNetSession = nullptr;
}

//  =========================================================================================
void NetConnection::SendPackets()
{
	NetSession* theNetSession = NetSession::GetInstance();

	while (m_latencySendTimer->ResetAndDecrementIfElapsed() && m_readyPackets.size() > 0)
	{		
		theNetSession->m_socket->SendTo(*m_address, m_readyPackets[0]->GetBuffer(), m_readyPackets[0]->GetWrittenByteCount());
		OnPacketSend(m_readyPackets[0]);
		m_sentPackets.push_back(m_readyPackets[0]);
		m_readyPackets.erase(m_readyPackets.begin());
	}
}

//  =========================================================================================
void NetConnection::OnPacketSend(NetPacket* packet)
{
	++m_nextSentAck;
	if (m_nextSentAck == INVALID_PACKET_ACK)
	{
		++m_nextSentAck;
	}

	m_lastSendTimeInHPC = GetMasterClock()->GetLastHPC();
	AddPacketTracker(packet->GetAck());
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
	uint16_t distance = header.m_ack - m_highestReceivedAck;
	if (distance == 0)
	{
		return;  //this should never happen as acks increment in order
	}

	if ((distance > 0))
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
	}	
}

//  =============================================================================
uint16_t NetConnection::GetNextAckToSend()
{
	return m_nextSentAck;
}

//  =============================================================================
void NetConnection::AddPacketTracker(uint16_t ack)
{
	PacketTracker tracker;
	tracker.m_ack = ack;
	tracker.m_sendTime = GetMasterClock()->GetLastHPC();

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

	m_trackedPackets[index] = tracker;
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
