#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Net\PacketTracker.hpp"
#include "Engine\Net\NetMessageChannel.hpp"
#include "Engine\Time\Stopwatch.hpp"

#define MAX_TRACKED_PACKETS (64)
#define MAX_UNIQUE_ID_LENGTH (16)
#define DEFAULT_PORT_RANGE (5)
#define INVALID_CONNECTION_INDEX (UINT8_MAX)

constexpr uint16_t RELIABLE_WINDOW(64);

enum eNetConnectionState
{
	CONNECTION_DISCONNECTED,
	CONNECTION_CONNECTED,
	CONNECTION_CONNECTING,
	CONNECTION_READY
};

struct NetConnectionInfo
{
	NetAddress* m_address = nullptr;
	char m_uniqueId[MAX_UNIQUE_ID_LENGTH];
	uint8_t m_connectionIndex = INVALID_CONNECTION_INDEX;
};

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	//info getters
	inline NetAddress* GetNetAddress() const { return m_info.m_address; }
	inline std::string GetUniqueIdAsString() const { std::string outputString(m_info.m_uniqueId); return outputString;}
	inline uint8_t GetConnectionIndex() const { return m_info.m_connectionIndex; }

	//info setters
	inline void SetNetAddress(NetAddress* address) { m_info.m_address = address; }
	inline void SetUniqueId(const char* uniqueId) { strcpy_s(m_info.m_uniqueId, MAX_UNIQUE_ID_LENGTH, uniqueId ); }
	inline void SetConnectionIndex(uint8_t index) { m_info.m_connectionIndex = index; }

	//send
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
	bool IsMe() const;
	bool IsHost() const;
	bool IsClient() const;

	inline bool IsConnected() const { return m_state == CONNECTION_CONNECTED; }
	inline bool IsDisconnected() const { return m_state == CONNECTION_DISCONNECTED; }
	inline bool IsConnecting() const { return m_state == CONNECTION_CONNECTING; }
	inline bool IsReady() const { return m_state == CONNECTION_READY; }

	// reliable helpers
	void MarkReliableReceived(uint16_t id);  //receive side
	bool HasReceivedReliableId(uint16_t id);

	void ConfirmSentReliable(uint16_t id);	 //send side	
	bool CanSendNewReliableMessage();
	uint64_t GetResendThresholdInHPC();

	//sequence helpers
	uint8_t OpenNewMessageChannel();
	void CloseMessageChannel(uint8_t channelIndex);
	uint16_t GetAndIncrementNextSequenceIdForChannel(uint8_t channelIndex);

public:
	//connection information ----------------------------------------------
	NetConnectionInfo m_info;
	eNetConnectionState m_state; 
	
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
	float m_connectionSendLatencyInMilliseconds = 0.f;
	float m_connectionResendRateInMilliseconds = NET_RELIABLE_RESEND_RATE_PER_MILLISECOND;
	
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

	NetMessageChannel* m_netMessageChannels[MAX_MESSAGE_CHANNELS];

	std::vector<NetPacket*> m_sentPackets;	
	PacketTracker* m_trackedPackets[MAX_TRACKED_PACKETS];
};