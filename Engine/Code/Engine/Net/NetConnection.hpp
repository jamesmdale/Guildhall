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
	uint16_t GetNextAckToSend();

	//getter helpers
	float GetRoundTripTimeInSeconds();
	float GetLossPercentage();
	float GetLastReceivedTimeInSeconds();
	float GetLastSentTimeInSeconds();
	int GetLastSentAck();
	int GetLastReceivedAck();

public:
	uint8_t m_index = UCHAR_MAX; //max of 255
	NetAddress* m_address = nullptr;
	float m_connectionSendLatencyInMilliseconds = 0;
	
	//sending = updated duringa  send/flush
	uint16_t m_nextSentAck = 0U;

	// receiving - during a process packet
	uint16_t m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t m_previousReceivedAckBitfield = 0U;

	//Analystics
	uint m_lastSendTimeInMilliseconds = 0U;
	uint m_lastReceivedTimeInMilliseconds = 0U;

	//reflects numbers from debug simulation on session
	float m_loss = 0.f; //loss rate we perceive
	float m_rtt = 0.f;	//latency perceived on this connection
	
	//timers
	Stopwatch* m_latencySendTimer = nullptr;
	Stopwatch* m_heartbeatTimer = nullptr;

	//storage for packets
	std::vector<NetMessage*> m_outgoingUnreliableMessages;
	std::vector<NetPacket*> m_readyPackets;
	std::vector<NetPacket*> m_sentPackets;	
	std::vector<TrackedPacket*> m_trackedPackets;
	std::vector<uint16_t>  m_sentPacketAcks;
};