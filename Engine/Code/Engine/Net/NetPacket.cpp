#include "NetPacket.hpp"

//  =============================================================================
NetPacket::NetPacket()
{
	m_packetHeader = new NetPacketHeader();
}

//  =============================================================================
NetPacket::~NetPacket()
{
	if (m_packetHeader != nullptr)
	{
		delete(m_packetHeader);
		m_packetHeader = nullptr;
	}	
}

//  =============================================================================
NetPacket::NetPacket(uint8_t senderIndex, uint8_t messageCount)
{
	m_packetHeader = new NetPacketHeader();
	m_packetHeader->m_senderIndex = senderIndex;
	m_packetHeader->m_messageCount = messageCount;
}

//  =============================================================================
bool NetPacket::ReadHeader(NetPacketHeader& packetHeader)
{
	return ReadBytes(&packetHeader, sizeof(NetPacketHeader), false);
}

//  =============================================================================
bool NetPacket::WriteMessage(NetMessage& netMessage)
{
	//write size
	bool success = false;

	success = WriteBytes(netMessage.GetBufferSize(), netMessage.GetBuffer(), false);

	//we succeeded so update message count and updat the header info
	if (success)
	{
		m_packetHeader->m_messageCount++;
		WriteUpdatedHeaderData();
	}

	return success;
}

//  =============================================================================
bool NetPacket::ReadMessage(NetMessage& netMessage)
{
	bool success = false;

	//get total message size
	uint16_t totalMessageSize;
	success = ReadBytes(&totalMessageSize, sizeof(uint16_t), false);
	if(!success)
		return false;

	//get message header contents
	NetMessageHeader messageHeaderContents;
	success = ReadBytes(&messageHeaderContents, sizeof(NetMessageHeader), false);
	if(!success)
		return false;

	//get total payload size and write that many bytes into netmessage
	uint16_t totalPayloadSize = totalMessageSize - sizeof(NetMessageHeader);	
	//success = netMessage.ReadBytes(&totalPayloadSize, GetBuffer(), false);

	return success;
}

//  =============================================================================
void NetPacket::WriteUpdatedHeaderData()
{
	//get written byte count to preserve count before we move the head
	size_t byteCount = GetBufferSize() - 1;

	//set write head to 0
	ResetWrite();

	//update the packet data with the current packet header count
	WriteBytes(sizeof(NetPacketHeader), (void*)m_packetHeader, false);

	//reset the write head back to 0
	ResetWrite();

	//move the write head back to it's original position
	MoveWriteHead(byteCount);	
}

//  =============================================================================
bool NetPacket::CheckIsValid()
{
	bool success = false;
	ResetHeads();
	
	//move write head to the end of the buffer
	size_t totalBufferSize = GetBufferSize();
	MoveWriteHead(totalBufferSize);

	NetPacketHeader headerData;
	success = ReadBytes(&headerData, sizeof(headerData), false);
	if (!success)
	{
		ResetHeads();
		return success;
	}
	
	int remainingPacketSize = (int)(totalBufferSize - sizeof(NetPacketHeader));
	
	for (int messageIndex = 0; messageIndex < (int)headerData.m_messageCount; ++messageIndex)
	{
		//read message header
		uint16_t totalMessageSize = 4;
		success = ReadBytes(&totalMessageSize, sizeof(uint16_t), false);
		if (!success)
		{
			ResetHeads();
			return success;
		}

		//message index
		//NetMessageHeader netMessageHeader;
		//success = ReadBytes(&netMessageHeader, sizeof(NetMessageHeader), false);
		//if (!success)
		//{
		//	ResetHeads();
		//	return success;
		//}

		////read total payload size
		//size_t payloadSize = totalMessageSize - sizeof(netMessageHeader);
		//void* payload = malloc(payloadSize);
		//success = ReadBytes(&payload, payloadSize, false);
		//if (!success)
		//{
		//	ResetHeads();
		//	return success;
		//}

		//get the total message size by adding the size of the header + size of the payload size + actual payload size
		//int totalmessageSize = (int)(sizeof(messageHeader) + sizeof(payloadSize) + payloadSize);

		if (remainingPacketSize - totalMessageSize >= 0)
		{
			remainingPacketSize -= totalMessageSize;
			MoveReadHead(totalMessageSize);			
		}
		else
		{
			ResetHeads();
			return false;
		}
	}

	//we read through the entire packet and our message count and sizes are consistent with total size of Packet
	ResetHeads();
	return true;	
}
