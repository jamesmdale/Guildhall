#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\NetMessage.hpp"

class NetPacketHeader
{
public:
	uint8_t m_senderIndex = UINT8_MAX;
	uint8_t m_messageCount = UINT8_MAX;
};

class NetPacket : public BytePacker
{
public:
	NetPacket();
	~NetPacket();

	NetPacket(uint8_t senderIndex, uint8_t messageCount);

	//Need to reset buffer when processin a whole packet
	bool ReadHeader(NetPacketHeader& packetHeader);

	bool WriteMessage(NetMessage& netMessage);
	bool ReadMessage(NetMessage& netMessage);

	void WriteUpdatedHeaderData();
	 
	bool CheckIsValid();

public:
	NetPacketHeader m_packetHeader;
};