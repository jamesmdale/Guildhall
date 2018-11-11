#pragma once
#include "Engine\Net\NetPacket.hpp"

class PacketTracker
{
public:
	PacketTracker();
	~PacketTracker();

public:
	uint16_t m_ack = UINT16_MAX;
	uint64_t m_sendTime = UINT64_MAX;
	uint16_t m_sentReliables[MAX_RELIABLES_PER_PACKET];
};

