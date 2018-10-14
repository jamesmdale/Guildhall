#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetPacket.hpp"

//  =============================================================================
NetConnection::NetConnection()
{
	m_connectionSendLatencyInMilliseconds = 0.f;
	m_sendWatch = new Stopwatch(GetMasterClock());

	NetSession* theNetSession = NetSession::GetInstance();

	if (theNetSession->m_sessionSendLatencyInMilliseconds > m_connectionSendLatencyInMilliseconds)
	{
		m_sendWatch->SetTimerInMilliseconds(theNetSession->m_sessionSendLatencyInMilliseconds);
	}
	else
	{
		m_sendWatch->SetTimerInMilliseconds(m_connectionSendLatencyInMilliseconds);
	}
		
}

//  =============================================================================
NetConnection::~NetConnection()
{
	delete(m_sendWatch);
	m_sendWatch = nullptr;
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

void NetConnection::SendPackets()
{
	NetSession* theNetSession = NetSession::GetInstance();

	while (m_sendWatch->ResetAndDecrementIfElapsed() && m_readyPackets.size() > 0)
	{		
		theNetSession->m_socket->SendTo(*m_address, m_readyPackets[0].GetBuffer(), m_readyPackets[0].GetWrittenByteCount());
		m_readyPackets.erase(m_readyPackets.begin());
	}
}
