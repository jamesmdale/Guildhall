#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetPacket.hpp"

//  =============================================================================
NetConnection::NetConnection()
{
}

//  =============================================================================
NetConnection::~NetConnection()
{
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

	NetSession* theNetSession = new NetSession();
	for (int messageIndex = 0; messageIndex < (int)m_outgoingMessages.size(); ++messageIndex)
	{
		//pack all of the NetMessages
		m_outgoingMessages[messageIndex]->ResetWrite();
		m_outgoingMessages[messageIndex]->WriteBytes(sizeof(uint16_t), (void*)(uint16_t)m_outgoingMessages[messageIndex]->GetBufferSize(), false);
	}

	//we have messages to send so start at the packet header
	NetPacket* packet = new NetPacket();

	packet->m_packetHeader.m_senderIndex = m_index;
	packet->m_packetHeader.m_messageCount =

	//once packet is 
	theNetSession->m_socket->SendTo(, byteCount);

}
