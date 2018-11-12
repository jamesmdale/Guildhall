#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetMessage.hpp"

class NetConnection;

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

	bool WriteMessage(NetMessage& netMessage, NetConnection* connection, uint16_t nextReliableId = UINT16_MAX);
	bool ReadMessage(NetMessage& netMessage);

	void WriteUpdatedPacketHeaderData();
	 
	bool CheckIsValid();
	uint16_t GetAck();

public:
	NetPacketHeader m_packetHeader;
};