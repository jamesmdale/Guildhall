#pragma once
#include "Engine\Core\BytePacker.hpp"

class NetPacket : public BytePacker
{
public:
	NetPacket();
	~NetPacket();

	NetPacket(uint8_t senderIndex, uint8_t messageCount);

	/*void WriteMessage();
	void ReadMessage();
	void CheckIsValid();*/

public:
	uint8_t m_senderIndex;
	uint8_t m_messageCount;

};