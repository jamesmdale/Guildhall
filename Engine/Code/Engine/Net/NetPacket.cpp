#include "NetPacket.hpp"

//  =============================================================================
NetPacket::NetPacket()
{
}

//  =============================================================================
NetPacket::~NetPacket()
{
}

//  =============================================================================
NetPacket::NetPacket(uint8_t senderIndex, uint8_t messageCount)
{
	m_packetHeader.m_senderIndex = senderIndex;
	m_packetHeader.m_messageCount = messageCount;
}

//  =============================================================================
void NetPacket::WriteHeader(const NetPacketHeader& packetHeader)
{
	WriteBytes(sizeof(packetHeader), &packetHeader, false);
}

//  =============================================================================
bool NetPacket::ReadHeader(NetPacketHeader& packetHeader)
{
	return ReadBytes(&packetHeader, sizeof(NetPacketHeader), false);
}

//  =============================================================================
bool NetPacket::WriteMessage(const NetMessage& netMessage)
{
	bool success = false;

	//write size
	uint16_t messageSize = netMessage.GetBufferSize() + sizeof(netMessage.m_header);	
	success = WriteBytes(sizeof(uint16_t), &messageSize, false);
	if(!success)
		return false;

	WriteBytes(sizeof(netMessage.m_header), &netMessage.m_header, false);
	if(!success)
		return false;

	WriteBytes(netMessage.GetBufferSize(), netMessage.GetBuffer(), false);
	if(!success)
		return false;

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
	success = netMessage.WriteBytes(totalPayloadSize, GetBuffer(), false);

	return success;
}

//  =============================================================================
bool NetPacket::CheckIsValid()
{
	bool success = false;
	ResetHeads();

	size_t totalBufferSize = GetBufferSize();

	NetPacketHeader headerData;
	success = ReadBytes(&headerData, sizeof(headerData), false);
	if (!success)
	{
		ResetHeads();
		return success;
	}
	
	int remainingMessageSize = (int)(totalBufferSize - sizeof(NetPacketHeader));
	
	for (int messageIndex = 0; messageIndex < (int)headerData.m_messageCount; ++messageIndex)
	{
		//read message header
		uint8_t messageHeader;
		success = ReadBytes(&messageHeader, sizeof(NetMessageHeader), false);
		if (!success)
		{
			ResetHeads();
			return success;
		}

		//read total payload size
		uint8_t payloadSize;
		success = ReadBytes(&payloadSize, sizeof(uint8_t), false);
		if (!success)
		{
			ResetHeads();
			return success;
		}

		//get the total message size by adding the size of the header + size of the payload size + actual payload size
		int totalmessageSize = (int)(sizeof(messageHeader) + sizeof(payloadSize) + payloadSize);

		if (remainingMessageSize - totalmessageSize >= 0)
		{
			remainingMessageSize -= totalmessageSize;
			MoveReadHead(payloadSize);			
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
