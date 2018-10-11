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

	NetSession* theNetSession = NetSession::GetInstance();

	bool areMessagesPacked = false;
	int currentMessageIndex = 0;

	std::vector<NetPacket*> packetsToSend;
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
			packetsToSend.push_back(packet);

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
			packetsToSend.push_back(packet);
		}
		else
		{
			currentMessageIndex++;		
		}
	}

	//send each packet
	for(int packetIndex = 0; packetIndex < (int)packetsToSend.size(); ++packetIndex)
	{
		theNetSession->m_socket->SendTo(*m_address, packetsToSend[packetIndex]->GetBuffer(), packetsToSend[packetIndex]->GetWrittenByteCount());
	}

	//cleanup outgoing message queue
	packetsToSend.clear();
	m_outgoingMessages.clear();

	/*delete(packet);
	packet = nullptr;*/

	theNetSession = nullptr;
}
