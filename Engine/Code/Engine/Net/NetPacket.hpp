#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\NetMessage.hpp"

#define INVALID_SENDER_ID (UINT8_MAX)
#define INVALID_PACKET_ACK (UINT16_MAX)
#define MAX_RELIABLES_PER_PACKET (32)

struct NetPacketHeader
{
public:
	uint8_t m_senderIndex = UINT8_MAX;

	uint16_t m_ack = UINT16_MAX;
	uint16_t m_highestReceivedAck = UINT16_MAX;
	uint16_t m_receivedAckHistoryBitfield = 0;

	uint8_t m_messageCount = 0;
};

class NetPacket : public BytePacker
{
public:
	NetPacket();
	~NetPacket();

	NetPacket(const NetPacketHeader & header);

	NetPacket(uint8_t senderIndex, uint8_t messageCount);

	//Need to reset buffer when processin a whole packet
	bool ReadHeader(NetPacketHeader& packetHeader);

	bool WriteMessage(NetMessage& netMessage, uint16_t nextReliableId = UINT16_MAX);
	bool ReadMessage(NetMessage& netMessage);

	void WriteUpdatedPacketHeaderData();
	 
	bool CheckIsValid();
	uint16_t GetAck();

public:
	NetPacketHeader m_packetHeader;
};