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

	void WriteHeader(const NetPacketHeader& packetHeader);
	bool ReadHeader(NetPacketHeader& packetHeader);

	bool WriteMessage(const NetMessage& netMessage);
	bool ReadMessage(NetMessage& netMessage);

	bool CheckIsValid();

public:
	NetPacketHeader m_packetHeader;
};