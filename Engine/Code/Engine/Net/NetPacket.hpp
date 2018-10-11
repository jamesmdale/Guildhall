#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\NetMessage.hpp"

class NetPacketHeader
{
public:
	uint8_t m_senderIndex;
	uint8_t m_messageCount;
};

class NetPacket : public BytePacker
{
public:
	NetPacket();
	~NetPacket();

	NetPacket(uint8_t senderIndex, uint8_t messageCount);

	//Need to reset buffer when processin a whole packet
	bool ReadHeader(NetPacketHeader& packetHeader);

	bool WriteMessage(NetMessage netMessage);
	bool ReadMessage(NetMessage& netMessage);

	void WriteUpdatedHeaderData();

	bool CheckIsValid();

public:
	NetPacketHeader* m_packetHeader = nullptr;
};