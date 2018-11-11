#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Net\PacketTracker.hpp"
#include "Engine\Time\Stopwatch.hpp"

#define MAX_TRACKED_PACKETS (64)

constexpr uint16_t RELIABLE_WINDOW(64);

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void QueueMessage(NetMessage* message);
	void FlushOutgoingMessages();
	void SendPacket(PacketTracker* packetTracker, NetPacket* packet);

	bool DoesHaveMessagesToSend();
	
	//state management on send
	void OnPacketSend(PacketTracker* packetTracker, NetPacket* packet);
	void OnReliableSend();
	uint16_t GetNextAckToSend();
	void AddPacketTracker(PacketTracker* tracker, uint16_t ack);

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

	// reliable helpers
	void MarkReliableReceived(uint16_t id);  //receive side
	bool HasReceivedReliableId(uint16_t id);

	void ConfirmSentReliable(uint16_t id);	 //send side	
	bool CanSendNewReliableMessage();
	uint64_t GetResendThresholdInHPC();

public:
	uint8_t m_index = UINT8_MAX; //max of 255
	NetAddress* m_address = nullptr;
	float m_connectionSendLatencyInMilliseconds = 0.f;
	float m_connectionResendRateInMilliseconds = NET_RELIABLE_RESEND_RATE_PER_MILLISECOND;
	
	// sending = updated duringa  send/flush ----------------------------------------------
	uint16_t m_nextSentAck = 0U;
	uint64_t m_lastSendTimeInHPC = 0U;

	uint16_t m_nextSentReliableId = 0;

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

	// reliable ----------------------------------------------
	std::vector<uint16_t> m_receivedReliableIds;
	uint16_t m_highestReceivedReliableId = UINT16_MAX;

	// storage for messages ----------------------------------------------
	std::vector<NetMessage*> m_unsentUnreliableMessages;
	std::vector<NetMessage*> m_unsentReliableMessages;
	std::vector<NetMessage*> m_unconfirmedSentReliablesMessages;

	std::vector<NetPacket*> m_sentPackets;	
	PacketTracker* m_trackedPackets[MAX_TRACKED_PACKETS];
};