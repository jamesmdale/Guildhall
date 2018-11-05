#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetPacket.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\Command.hpp"
#include <map>
#include <string>
#include <vector>

#define MAX_NET_DEFINITION_REGISTRATIONS (UINT8_MAX)
#define RELIABLE_WINDOW (32)

enum eCoreNetMessageType
{
	PING_CORE_NET_MESSAGE_TYPE = 0,
	PONG_CORE_NET_MESSAGE_TYPE,
	HEARTBEAT_CORE_NET_MESSAGE_TYPE,
	NUM_CORE_NET_MESSAGE_TYPES
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

	bool BindPort(uint port, uint range);
	bool AddConnection(uint8_t connectionIndex, NetAddress* address);	

	// message processing ----------------------------------------------
	
	// outgoing
	void CheckHeartbeats();
	void SendHeartBeat(int connectionIndex);
	void ProcessOutgoingMessages();
	bool SendMessageWithoutConnection(NetMessage* message, NetConnection* connection);

	//incoming
	void ProcessIncomingMessages();
	void CheckDelayedPackets();
	void ProcessDelayedPacket(DelayedReceivedPacket* packet);	

	// message registration ----------------------------------------------
	bool RegisterMessageDefinition(const std::string& name, NetMessageCallback callback);
	bool RegisterMessageDefinition(int netMessageId, const std::string& name, NetMessageCallback callback, const eNetMessageFlag& flag = (eNetMessageFlag)0);
	void LockMessageDefinitionRegistration();
	void SortMessageDefinitionsByName();

	void AssignFinalDefinitionIds();

	NetConnection* GetConnectionById(uint8_t id);

	// helpers ----------------------------------------------
	void SetHeartbeatRate(float hertz);

	// simulation latency helpers ----------------------------------------------
	void SetSimulatedLossAmount(float lossAmount);
	void SetSimulatedLatency(uint minLatencyInMilliseconds, uint maxLatencyInMilliseconds);

public:
	UDPSocket* m_socket = nullptr;
	std::vector<NetConnection*> m_connections;
	uint8_t m_sessionConnectionIndex = UINT8_MAX;

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
