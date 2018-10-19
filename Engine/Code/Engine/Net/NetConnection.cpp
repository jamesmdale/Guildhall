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
}

//  =============================================================================
void NetConnection::QueueMessage(NetMessage* message)
{
	m_outgoingMessages.push_back(message);
}

//  =============================================================================
void NetConnection::FlushOutgoingMessages()
{
	if(m_outgoingMessages.size() == 0)
		return;

	NetSession* theNetSession = NetSession::GetInstance();

	bool areMessagesPacked = false;
	int currentMessageIndex = 0;

	//setup header
	NetPacketHeader header;
	header.m_senderIndex = theNetSession->m_sessionConnectionIndex;
	header.m_ack = GetNextAckToSend();
	header.m_lastReceivedAck = m_lastReceivedAck;
	header.m_receivedAckBitfield = m_previousReceivedAckBitfield;

	NetPacket* packet = new NetPacket(theNetSession->m_sessionConnectionIndex, 0);
	packet->WriteUpdatedHeaderData();
	
	while (!areMessagesPacked)
	{
		//total buffer size + payload size + header size + total message size (header + payload sizes)
		if (packet->GetBufferSize() + m_outgoingMessages[currentMessageIndex]->GetWrittenByteCount() + sizeof(NetMessageHeader) + sizeof(uint16_t) <= PACKET_MTU)
		{
			//write message to packet
			packet->WriteMessage(*m_outgoingMessages[currentMessageIndex]);
		}
		else
		{
			//in order to pack the next message, we need to make a new packet
			//first add old packet to list
			m_readyPackets.push_back(*packet);

			//make a new packet
			delete(packet);
			packet = new NetPacket(theNetSession->m_sessionConnectionIndex, 0);

			//write new message to new packet
			packet->WriteMessage(*m_outgoingMessages[currentMessageIndex]);
		}

		//if we have read all the messages we are done
		if (currentMessageIndex == (int)m_outgoingMessages.size() - 1)
		{
			areMessagesPacked = true;
			
			//add the last packet to the list to send
			m_readyPackets.push_back(*packet);
		}
		else
		{
			currentMessageIndex++;		
		}
	}

	m_outgoingMessages.clear();

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
		theNetSession->m_socket->SendTo(*m_address, m_readyPackets[0].GetBuffer(), m_readyPackets[0].GetWrittenByteCount());
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
	
	m_sentPacketAcks.push_back(packet->GetAck());
}

//  =========================================================================================
void NetConnection::OnReceivePacket(uint16_t ack)
{
	for (int packetAckIndex = 0; packetAckIndex < (int)m_sentPacketAcks.size(); ++packetAckIndex)
	{
		if (ack == m_sentPacketAcks[packetAckIndex])
		{
			DevConsole::GetInstance()->DevConsolePrintf("Packet Received: %u", ack);
		}
	}
}

//  =========================================================================================
void NetConnection::OnAckReceived(uint16_t ack)
{

}
