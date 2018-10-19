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

	void OnPacketSend(NetPacket* packet);
	void OnReceivePacket(uint16_t ack);

	void OnAckReceived(uint16_t ack);

public:
	uint8_t m_index = UCHAR_MAX; //max of 255
	NetAddress* m_address = nullptr;
	float m_connectionSendLatencyInMilliseconds = 0;
	
	//sending = updated duringa  send/flush
	uint16_t m_nextSentAck = 0U;

	// receiving - during a process packet
	uint16_t m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t m_previousReceivedAckBitfield = 0U;
	std::vector<uint16_t>  m_sentPacketAcks;

	//Analystics
	uint m_lastSendTimeInMilliseconds = 0U;
	uint m_lastReceivedTimeInMilliseconds = 0U;

	//reflects numbers from debug simulation on session
	float m_loss = 0.f; //loss rate we perceive
	float m_rtt = 0.f;	//latency perceived on this connection

	
	Stopwatch* m_latencySendTimer = nullptr;
	Stopwatch* m_heartbeatTimer = nullptr;

	std::vector<NetMessage*> m_outgoingMessages;
	std::vector<NetPacket> m_readyPackets;

	std::vector<NetPacket*> m_sentPackets;
};