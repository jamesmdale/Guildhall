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
	m_heartbeatTimer->SetTimer(0.f);
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
	header.m_highestReceivedAck = m_theirHighestReceivedAck;
	header.m_highestReceivedAckBitfield = m_theirHighestReceivedAckBitfield;

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
			header.m_highestReceivedAck = m_theirHighestReceivedAck;
			header.m_highestReceivedAckBitfield = m_theirHighestReceivedAckBitfield;

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

	AddPacketTracker(packet->GetAck());
}

//  =========================================================================================
void NetConnection::OnReceivePacket(NetPacket* packet)
{
	NetPacketHeader header = packet->m_packetHeader;

	//process the ack they sent me
	m_myLastReceivedTimeInHPC = GetMasterClock()->GetLastHPC();

	//update state based on what they say their highest received and bitfield history is
	OnAckReceived(header.m_highestReceivedAck);
}

//  =========================================================================================
void NetConnection::OnAckReceived(uint16_t ack)
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
		float m_rtt = PerformanceCounterToMilliseconds(rttInHPC);

		//do stuff with the bitfield to confirm 
		m_myLastReceivedTimeInHPC = GetMasterClock()->GetLastHPC();
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

	m_trackedPackets[index] = tracker;
}

//  =============================================================================
float NetConnection::GetRoundTripTimeInSeconds()
{
	return 0.0f;
}

//  =============================================================================
float NetConnection::GetLossPercentage()
{
	return 0.0f;
}

//  =============================================================================
float NetConnection::GetLastReceivedTimeInSeconds()
{
	return m_myLastReceivedTimeInHPC * 1000.f;
}

//  =============================================================================
float NetConnection::GetLastSentTimeInSeconds()
{
	return m_lastSendTimeInHPC * 1000.f;
}

//  =============================================================================
int NetConnection::GetLastSentAck()
{
	return m_nextSentAck - 1;
}

//  =============================================================================
int NetConnection::GetLastReceivedAck()
{
	return m_theirHighestReceivedAck;
}
