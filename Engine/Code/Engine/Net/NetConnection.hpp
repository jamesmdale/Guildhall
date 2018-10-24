#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Time\Stopwatch.hpp"

#define MAX_TRACKED_PACKETS (64)

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void QueueMessage(NetMessage* message);
	void FlushOutgoingMessages();
	void SendPackets();
	
	//state management on send
	void OnPacketSend(NetPacket* packet);
	uint16_t GetNextAckToSend();
	void AddPacketTracker(uint16_t ack);

	//state manager on receive
	void OnReceivePacket(NetPacket* packet);
	void OnMyAckReceived(uint16_t ack);

	//getter helpers
	float GetRoundTripTimeInSeconds();
	float UpdateLossPercentage();
	float GetLossPercentage();
	float GetLastReceivedTimeInSeconds();
	float GetLastSentTimeInSeconds();
	int GetLastSentAck();
	int GetLastReceivedAck();

public:
	uint8_t m_index = UINT8_MAX; //max of 255
	NetAddress* m_address = nullptr;
	float m_connectionSendLatencyInMilliseconds = 0;
	
	// sending = updated duringa  send/flush ----------------------------------------------
	uint16_t m_nextSentAck = 0U;
	uint64_t m_lastSendTimeInHPC = 0U;

	// receiving - during a process packet ----------------------------------------------
	uint16_t m_highestReceivedAck = INVALID_PACKET_ACK;
	uint16_t m_receivedAckHistoryBitfield = 0U;
	uint64_t m_myLastReceivedTimeInHPC = 0U; //last time I received anything (regardless of ack)

	// reflects numbers from debug simulation on session ----------------------------------------------
	int m_numLostPackets = 0; //loss rate we perceive
	float m_lossPercentage = 0.f;
	float m_rttInMilliseconds = -1.f;	//latency perceived on this connection
	
	// timers ----------------------------------------------
	Stopwatch* m_latencySendTimer = nullptr;
	Stopwatch* m_heartbeatTimer = nullptr;

	// storage for packets ----------------------------------------------
	std::vector<NetMessage*> m_outgoingUnreliableMessages;
	std::vector<NetPacket*> m_readyPackets;
	std::vector<NetPacket*> m_sentPackets;	
	PacketTracker m_trackedPackets[MAX_TRACKED_PACKETS];
};