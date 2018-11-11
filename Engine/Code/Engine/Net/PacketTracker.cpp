#include "Engine\Net\PacketTracker.hpp"


PacketTracker::PacketTracker()
{
	for (int reliableIndex = 0; reliableIndex < MAX_RELIABLES_PER_PACKET; ++reliableIndex)
	{
		m_sentReliables[reliableIndex] = UINT16_MAX;
	}
}


PacketTracker::~PacketTracker()
{
}
