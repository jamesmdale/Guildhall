#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <map>
#include <string>
#include <vector>


#define MAX_NUM_NET_CONNECTIONS (17)
#define MAX_NET_TIME_DILATION (0.1f)		// this controls how much the clock is allowed to speed up/slow down to match a snapshot

constexpr float DEFAULT_CONNECTION_TIMEOUT_IN_SECONDS(100.f);

enum eCoreNetMessageType
{
	PING_CORE_NET_MESSAGE_TYPE = 0,
	PONG_CORE_NET_MESSAGE_TYPE,
	HEARTBEAT_CORE_NET_MESSAGE_TYPE,
	JOIN_REQUEST_CORE_NET_MESSAGE_TYPE,
	JOIN_DENY_CORE_NET_MESSAGE_TYPE,
	JOIN_ACCEPT_CORE_NET_MESSAGE_TYPE,
	NEW_CONNECTION_CORE_NET_MESSAGE_TYPE, 
	JOIN_FINISHED_CORE_NET_MESSAGE_TYPE, 
	UPDATE_CONNECTION_STATE_CORE_NET_MESSAGE_TYPE,
	HANGUP_CORE_NET_MESSAGE_TYPE,
	NUM_CORE_NET_MESSAGE_TYPES
};

enum eNetSessionState
{
	SESSION_STATE_DISCONNECTED = 0,		//Session can be modified
	SESSION_STATE_BOUND,					//Bound to a socket. (can send a receive connecitonless messages. No connections exist)
	SESSION_STATE_CONNECTING,				// Attempting to connect. (waiting for response from host)
	SESSION_STATE_JOINING,				// has established a connection. waiting for final setup information/join completion
	SESSION_STATE_READY					// we are fully in the session
};

enum eNetSessionError
{
	SESSION_OK = 0,
	SESSION_ERROR_USER,					//user disconnected
	SESSION_ERROR_INTERNAL,				//socket error
	SESSION_ERROR_JOIN_DENIED,			//generic error
	SESSION_ERROR_JOIN_DENIED_NOT_HOST,	//session tried to jion someone who isn't hosting
	SESSION_ERROR_JOIN_DENIED_CLOSED,	//session not in a listen state
	SESSION_ERROR_JOIN_DENIED_FULL		//session was full
};

struct DelayedReceivedPacket
{
	DelayedReceivedPacket(){}
	~DelayedReceivedPacket()
	{
		if (m_packet != nullptr)
		{
			delete(m_packet);
			m_packet = nullptr;
		}
	}

	uint64_t m_timeToProcess = 0.0;
	NetPacket* m_packet = nullptr;
	NetAddress m_senderAddress;
};

class NetSession
{
public:
	NetSession();
	~NetSession();
	static NetSession* GetInstance();
	static NetSession* CreateInstance();

	void Startup();
	void Shutdown();

	void Update(float deltaSeconds);
	void UpdateConnecting();
	void UpdateJoining();
	void UpdateReady();
	void RegisterCoreMessageTypes();

	void ResetConnectionTimers();

	// connection and session state ----------------------------------------------
	int GetNumBoundConnections();
	int GetFirstUnboundConnectionIndex();

	void Host(const char* myId, uint16_t port, uint16_t portRange = 0);
	void Join(const char* myId, const NetConnectionInfo& hostInfo);
	void Disconnect();

	inline eNetSessionState GetState() const { return m_state; }
	inline void SetState(eNetSessionState state) { m_state = state; }

	bool IsDisconnected();
	bool IsJoined();

	void SetError(eNetSessionError error, const char* errorString = nullptr);
	void ClearError();
	eNetSessionError GetLastError(std::string* outErrorString = nullptr);

	std::string GenerateUniqueId();

	//connection methods
	NetConnection* CreateConnection(const NetConnectionInfo& info);
	void DestroyConnection(NetConnection* connection); 
	void BindConnection(uint8_t connectionIndex, NetConnection* connection);
	NetConnection* GetBoundConnectionByAddress(const NetAddress& address);
	int GetConnectionIndexFromAllConnectionsByAddress(const NetAddress & address);
	void CleanupConnections();

	// send and process incoming  ----------------------------------------------

	//heartbeat
	void CheckHeartbeats();
	void SendHeartBeat(int connectionIndex);

	//outgoing
	void ProcessOutgoingMessages();
	bool SendMessageWithoutConnection(NetMessage* message, NetConnection* connection);

	//incoming
	void ProcessIncomingMessages();
	void CheckDelayedPackets();
	void ProcessDelayedPacket(DelayedReceivedPacket* packet);	
	void ExecuteNetMessage(NetMessage* message, NetConnection* connection);

	// message registration ----------------------------------------------
	bool RegisterMessageDefinition(const std::string& name, NetMessageCallback callback);
	bool RegisterMessageDefinition(int netMessageId, const std::string& name, NetMessageCallback callback, const eNetMessageFlag& flag = (eNetMessageFlag)0, const uint8_t& messageChannelIndex = UINT8_MAX);
	void LockMessageDefinitionRegistration();
	void SortMessageDefinitionsByName();

	void AssignFinalDefinitionIds();

	NetConnection* GetBoundConnectionById(uint8_t id);

	// helpers ----------------------------------------------
	void SetHeartbeatRate(float hertz);
	void SetSimulatedLossAmount(float lossAmount);
	void SetSimulatedLatency(uint minLatencyInMilliseconds, uint maxLatencyInMilliseconds);
	void DisconnectNetSession();

	void UpdateNetClock(float deltaSeconds);
	uint GetNetTimeInMilliseconds();
	void UpdateClientNetClock(uint hostTime);

public:
	UDPSocket* m_socket = nullptr;
	NetConnection* m_boundConnections[MAX_NUM_NET_CONNECTIONS];
	std::vector<NetConnection*> m_allConnections;

	NetConnection* m_myConnection = nullptr;
	NetConnection* m_hostConnection = nullptr;

	//state management
private:
	eNetSessionState m_state = SESSION_STATE_DISCONNECTED;

public:
	eNetSessionError m_errorCode;
	std::string m_errorString;

	//simulation variables
	/*float m_simulationLossAmount = 0.2f;	
	uint m_minAddedLatencyInMilliseconds = 100;
	uint m_maxAddedLatencyInMilliseconds = 120;*/

	float m_simulationLossAmount = 0.0f;	
	uint m_minAddedLatencyInMilliseconds = 50;
	uint m_maxAddedLatencyInMilliseconds = 100;

	float m_sessionSendLatencyInMilliseconds = 0.1;

	// netclock variables ----------------------------------------------
	//ONLY USED BY CLIENT
	uint m_lastReceivedHostTimeInMilliseconds;	// this is the time we received from the host + (RTT / 2)
	uint m_desiredClientTimeInMilliseconds;		// Time we want the the client to eventually be
	uint m_currentClientTimeInMilliseconds = UINT_MAX;	  // what the client will actually report when returning

public:
	bool m_isDefinitionRegistrationLocked = false;

	int m_nextRegisteredDefinitionIndex = 0;
	static NetMessageDefinition* s_registeredMessageDefinitions[UINT8_MAX];
	//static std::map<std::string, NetMessageDefinition*> s_registeredMessageDefinitions;

	//delayed packets to process with latency
	std::vector<DelayedReceivedPacket*> m_delayedPackets;
};

// net callbacks ----------------------------------------------
NetMessageCallback GetRegisteredNetCallbackById(int id);
NetMessageCallback GetRegisteredCallbackByName(const std::string& name);
NetMessageDefinition* GetRegisteredDefinitionById(int id);
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string& name);

// console commands ----------------------------------------------
void AddConnectionToIndex(Command& cmd);
void SetToHost(Command& cmd);
void SetToDisconnect(Command& cmd);
void SetToJoin(Command& cmd);
void SendPing(Command& cmd);
void SendMultiPing(Command& cmd);
void SendAdd(Command& cmd);
void SetNetSimLag(Command& cmd);
void SetNetSimLoss(Command& cmd);
void SetSessionSendRate(Command& cmd);
void SetConnectionSendRate(Command& cmd);
void SetGlobalHeartRate(Command& cmd);

// message registrations ----------------------------------------------
bool OnPing(NetMessage& message, NetConnection* fromConnection);
bool OnPong(NetMessage& message, NetConnection* fromConnection);
bool OnAdd(NetMessage& message, NetConnection* fromConnection);
bool OnAddResponse(NetMessage& message, NetConnection* fromConnection);
bool OnHeartbeat(NetMessage& message, NetConnection* fromConnection);
bool OnAck(NetMessage& message, NetConnection* fromConnection);
bool OnUnreliableTest(NetMessage& message, NetConnection* fromConnection);

bool OnJoinRequest(NetMessage& message, NetConnection* fromConnection);
bool OnJoinDenied(NetMessage& message, NetConnection* fromConnection);
bool OnJoinAccepted(NetMessage& message, NetConnection* fromConnection);
bool OnNewConnection(NetMessage& message, NetConnection* fromConnection);
bool OnJoinFinished(NetMessage& message, NetConnection* fromConnection);
bool OnUpdateConnectionState(NetMessage& message, NetConnection* fromConnection);





