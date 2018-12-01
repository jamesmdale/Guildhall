#include "Engine\Net\NetPacket.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
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
	m_packetHeader.m_messageCount = messageCount;
}

//  =============================================================================
bool NetPacket::ReadHeader(NetPacketHeader& packetHeader)
{
	bool success = ReadBytes(&packetHeader.m_senderIndex, sizeof(uint8_t), false);
	if (!success)
	{
		return success;
	}		

	success = ReadBytes(&packetHeader.m_ack, sizeof(uint16_t), false);
	if (!success)
	{
		return success;
	}		

	success = ReadBytes(&packetHeader.m_highestReceivedAck, sizeof(uint16_t), false);
	if (!success)
	{
		return success;
	}		

	success = ReadBytes(&packetHeader.m_receivedAckHistoryBitfield, sizeof(uint16_t), false);
	if (!success)
	{
		return success;
	}		

	success = ReadBytes(&packetHeader.m_messageCount, sizeof(uint8_t), false);
	if (!success)
	{
		return success;
	}		

	return success;
}

//  =============================================================================
bool NetPacket::WriteMessage(NetMessage& netMessage, NetConnection* connection, uint16_t nextReliableId)
{
	//write size
	bool success = false;

	uint16_t totalMessageSize = sizeof(uint8_t) + netMessage.GetWrittenByteCount();

	//calculate size of message header
	if (netMessage.m_definition->IsReliable())
	{
		//add reliable id
		totalMessageSize += sizeof(uint16_t);
	}
	if (netMessage.m_definition->IsInOrder())
	{
		//add sequence id
		totalMessageSize += sizeof(uint16_t);
	}

	//write total size
	success = WriteBytes(sizeof(uint16_t), &totalMessageSize, false);

	//write message header callback index
	success = WriteBytes(sizeof(netMessage.m_header->m_messageCallbackDefinitionIndex), &netMessage.m_header->m_messageCallbackDefinitionIndex, false);

	//write message header reliable id IF the message is reliable
	if (netMessage.m_definition->IsReliable())
	{
		netMessage.m_header->m_reliableId = nextReliableId;
		success = WriteBytes(sizeof(netMessage.m_header->m_reliableId), &netMessage.m_header->m_reliableId, false);
	}

	//write message header sequence id IF the message is inorder
	if (netMessage.m_definition->IsInOrder())
	{
		uint8_t channelId = netMessage.m_definition->m_messageChannelIndex;

		ASSERT_RECOVERABLE(connection != nullptr, "CONNECTION INVALID: DEFINITION REQUIRES CONNECTION");

		netMessage.m_header->m_sequenceId = connection->GetAndIncrementNextSequenceIdForChannel(channelId);
		success = WriteBytes(sizeof(netMessage.m_header->m_sequenceId), &netMessage.m_header->m_sequenceId, false);
	}

	//write payload
	success = WriteBytes(netMessage.GetWrittenByteCount(), netMessage.GetBuffer(), false);

	//we succeeded so update message count and updat the header info
	if (success)
	{
		m_packetHeader.m_messageCount++;
		WriteUpdatedPacketHeaderData();
	}

	return success;
}

//  =============================================================================
bool NetPacket::ReadMessage(NetMessage& netMessage)
{
	bool success = false;

	////get total message size
	//uint16_t totalMessageSize;
	//success = ReadBytes(&totalMessageSize, sizeof(uint16_t), false);
	//if(!success)
	//	return false;

	////get message header contents
	//NetMessageHeader messageHeader;
	//size_t totalHeaderSize = 0;

	////read callbackid 
	//totalHeaderSize += sizeof(messageHeader.m_messageCallbackDefinitionIndex);
	//success = ReadBytes(&messageHeader.m_messageCallbackDefinitionIndex, sizeof(messageHeader.m_messageCallbackDefinitionIndex), false);	
	//if (!success)
	//{
	//	return false;
	//}

	////read reliableID IF the message is reliable 
	//if (netMessage.m_definition->IsReliable())
	//{
	//	totalHeaderSize += sizeof(messageHeader.m_reliableId);
	//	success = ReadBytes(&messageHeader.m_reliableId, sizeof(messageHeader.m_reliableId), false);
	//	if (!success)
	//	{
	//		return false;
	//	}
	//}		

	////read inorder IF the message is reliable 
	//if (netMessage.m_definition->IsInOrder())
	//{
	//	totalHeaderSize += sizeof(messageHeader.m_sequenceId);
	//	success = ReadBytes(&messageHeader.m_sequenceId, sizeof(messageHeader.m_sequenceId), false);
	//	if (!success)
	//	{
	//		return false;
	//	}
	//}		

	////get total payload size and write that many bytes into netmessage
	//uint16_t totalPayloadSize = totalMessageSize - totalHeaderSize;	

	return success;
}

//  =============================================================================
void NetPacket::WriteUpdatedPacketHeaderData()
{
	//set write head to 0
	ResetWrite();

	//update the packet data with the current packet header count
	WriteBytes(sizeof(uint8_t), &m_packetHeader.m_senderIndex, false);
	WriteBytes(sizeof(uint16_t), &m_packetHeader.m_ack, false);
	WriteBytes(sizeof(uint16_t), &m_packetHeader.m_highestReceivedAck, false);
	WriteBytes(sizeof(uint16_t), &m_packetHeader.m_receivedAckHistoryBitfield, false);
	WriteBytes(sizeof(uint8_t), &m_packetHeader.m_messageCount, false);

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
	success = ReadHeader(headerData);
	if (!success)
	{
		ResetRead();
		return success;
	}
	
	int remainingPacketSize = (int)(totalBufferSize - sizeof(NetPacketHeader));
	
	for (int messageIndex = 0; messageIndex < (int)headerData.m_messageCount; ++messageIndex)
	{
		//read message header
		uint16_t totalMessageSize;
		success = ReadBytes(&totalMessageSize, sizeof(uint16_t), false);
		if (!success)
		{
			ResetRead();
			return success;
		}

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
