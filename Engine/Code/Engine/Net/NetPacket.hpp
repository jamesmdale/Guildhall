#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\NetMessage.hpp"

#define INVALID_SENDER_ID (UINT8_MAX)
#define INVALID_PACKET_ACK (UINT16_MAX)

class NetPacketHeader
{
public:
	uint8_t m_senderIndex = UINT8_MAX;

	uint16_t m_ack = UINT16_MAX;
	uint16_t m_lastReceivedAck = UINT16_MAX;
	uint16_t m_receivedAckBitfield = UINT16_MAX;

	uint8_t m_unreliableMessageCount = UINT8_MAX;
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

	bool WriteMessage(NetMessage& netMessage);
	bool ReadMessage(NetMessage& netMessage);

	void WriteUpdatedHeaderData();

	bool CheckIsValid();
	uint16_t GetAck();

public:
	NetPacketHeader m_packetHeader;
};