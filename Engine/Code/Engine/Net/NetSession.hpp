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


#define MAX_NET_CONNECTION_COUNT (17)

enum eCoreNetMessageType
{
	PING_CORE_NET_MESSAGE_TYPE = 0,
	PONG_CORE_NET_MESSAGE_TYPE,
	HEARTBEAT_CORE_NET_MESSAGE_TYPE,
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

	void Update();
	void RegisterCoreMessageTypes();

	// connection and session state ----------------------------------------------
	void Host(const char* myId, uint16_t port, uint16_t portRange = 0);
	void Join(const char* myId, const NetConnectionInfo& hostInfo);
	void Disconnect();

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

	NetConnection* GetConnectionById(uint8_t id);

	// helpers ----------------------------------------------
	void SetHeartbeatRate(float hertz);
	void SetSimulatedLossAmount(float lossAmount);
	void SetSimulatedLatency(uint minLatencyInMilliseconds, uint maxLatencyInMilliseconds);

public:
	UDPSocket* m_socket = nullptr;
	std::vector<NetConnection*> m_boundConnections;
	std::vector<NetConnection*> m_allConnections;

	NetConnection* m_myConnection = nullptr;
	NetConnection* m_hostConnection = nullptr;

	uint8_t m_sessionConnectionIndex = UINT8_MAX;

	//state management
	eNetSessionState m_state = SESSION_STATE_DISCONNECTED;
	eNetSessionError m_errorCode;
	std::string m_errorString;

	//simulation variables
	float m_simulationLossAmount = 0.f;
	
	uint m_minAddedLatencyInMilliseconds = 50;
	uint m_maxAddedLatencyInMilliseconds = 100;

	float m_sessionSendLatencyInMilliseconds = 0.1;

	Stopwatch* m_heartbeatRate = nullptr;

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
void SetToHost(Command & cmd);
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
