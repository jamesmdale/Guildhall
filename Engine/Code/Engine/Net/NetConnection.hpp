#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Time\Stopwatch.hpp"

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void QueueMessage(NetMessage* message);
	void FlushOutgoingMessages();
	void SendPackets();

public:
	uint8_t m_index = UCHAR_MAX; //max of 255
	NetAddress* m_address = nullptr;
	float m_connectionSendLatencyInMilliseconds = 0;
	
	Stopwatch* m_latencySendTimer = nullptr;
	Stopwatch* m_heartbeatTimer = nullptr;

	std::vector<NetMessage*> m_outgoingMessages;
	std::vector<NetPacket> m_readyPackets;
};