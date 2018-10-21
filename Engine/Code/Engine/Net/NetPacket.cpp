#include "NetPacket.hpp"
#include <type_traits>

//  =============================================================================
NetPacket::NetPacket()
{
}

//  =============================================================================
NetPacket::~NetPacket()
{
}

//  =========================================================================================
NetPacket::NetPacket(const NetPacketHeader& header)
{
	m_packetHeader = header;
}

//  =============================================================================
NetPacket::NetPacket(uint8_t senderIndex, uint8_t messageCount)
{
	m_packetHeader.m_senderIndex = senderIndex;
	m_packetHeader.m_unreliableMessageCount = messageCount;
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

	//write total size
	uint16_t numSize = sizeof(NetMessageHeader) + netMessage.GetWrittenByteCount();
	success = WriteBytes(sizeof(uint16_t), &numSize, false);

	//write header
	success = WriteBytes(sizeof(NetMessageHeader), netMessage.m_header, false);

	//write payload
	success = WriteBytes(netMessage.GetWrittenByteCount(), netMessage.GetBuffer(), false);

	//we succeeded so update message count and updat the header info
	if (success)
	{
		m_packetHeader.m_unreliableMessageCount++;
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
	//set write head to 0
	ResetWrite();

	//update the packet data with the current packet header count
	WriteBytes(sizeof(uint8_t), &m_packetHeader.m_senderIndex, false);
	WriteBytes(sizeof(uint16_t) &m_packetHeader.m_ack, false);
	WriteBytes(sizeof(uint16_t) &m_packetHeader.m_highestReceivedAck, false);
	WriteBytes(sizeof(uint16_t) &m_packetHeader.m_highestReceivedAckBitfield, false);
	WriteBytes(sizeof(uint8_t), &m_packetHeader.m_unreliableMessageCount, false);

	//move the write head back to the end of the amount we've written
	SetWriteHeadToMaxWritten();	
}

//  =============================================================================
bool NetPacket::CheckIsValid()
{
	bool success = false;
	ResetHeads();
	
	//move write head to the end of the buffer
	size_t totalBufferSize = GetBufferSize();
	SetWrittenByteCountToBufferSize();
	SetWriteHeadToMaxWritten();

	NetPacketHeader headerData;
	success = ReadBytes(&headerData, sizeof(NetPacketHeader), false);
	if (!success)
	{
		ResetRead();
		return success;
	}
	
	int remainingPacketSize = (int)(totalBufferSize - sizeof(NetPacketHeader));
	
	for (int messageIndex = 0; messageIndex < (int)headerData.m_unreliableMessageCount; ++messageIndex)
	{
		//read message header
		uint16_t totalMessageSize;
		success = ReadBytes(&totalMessageSize, sizeof(uint16_t), false);
		if (!success)
		{
			ResetRead();
			return success;
		}

		////message index
		//NetMessageHeader netMessageHeader;
		//success = ReadBytes(&netMessageHeader, sizeof(NetMessageHeader), false);
		//if (!success)
		//{
		//	ResetHeads();
		//	return success;
		//}


		////read total payload size
		//size_t payloadSize = totalMessageSize - sizeof(uint16_t) - sizeof(netMessageHeader);

		//if (payloadSize > 0)
		//{
		//	void* payload = malloc(payloadSize);
		//	success = ReadBytes(&payload, payloadSize, false);
		//	if (!success)
		//	{
		//		ResetHeads();
		//		return success;
		//	}
		//}		

		//get the total message size by adding the size of the header + size of the payload size + actual payload size
		//int totalmessageSize = (int)(sizeof(netMessageHeader) + sizeof(payloadSize) + payloadSize);

		if (remainingPacketSize - totalMessageSize >= 0)
		{
			remainingPacketSize -= totalMessageSize;
			MoveReadHead(totalMessageSize);			
		}
		else
		{
			ResetRead();
			return false;
		}
	}

	//we read through the entire packet and our message count and sizes are consistent with total size of Packet
	ResetRead();
	return true;	
}

uint16_t NetPacket::GetAck()
{
	return m_packetHeader.m_ack;
}
