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

	void Update();

	void Startup();

	bool BindPort(uint port, uint range);
	bool AddConnection(uint8_t connectionIndex, NetAddress* address);	

	//message processing
	void ProcessIncomingMessages();
	void CheckDelayedPackets();
	void ProcessDelayedPacket(DelayedReceivedPacket* packet);
	void ProcessOutgoingMessages();

	bool SendMessageWithoutConnection(NetMessage* message, NetConnection* connection);

	//message registration
	bool RegisterMessageDefinition(const std::string& name, NetMessageCallback callback);
	void LockMessageDefinitionRegistration();

	NetConnection* GetConnectionById(uint8_t id);

	//simulation helpers
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

public:
	bool m_isDefinitionRegistrationLocked = false;
	static std::map<std::string, NetMessageDefinition*> s_registeredMessageDefinitions;

	//delayed packets to process with latency
	std::vector<DelayedReceivedPacket*> m_delayedPackets;
};

//net callbacks
NetMessageCallback GetRegisteredNetCallbackById(int id);
NetMessageCallback GetRegisteredCallbackByName(const std::string& name);
NetMessageDefinition* GetRegisteredDefinitionById(int id);
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string& name);

//console commands
void AddConnectionToIndex(Command& cmd);
void SendPing(Command& cmd);
void SendMultiPing(Command& cmd);
void SendAdd(Command& cmd);
void SetNetSimLag(Command& cmd);
void SetNetSimLoss(Command& cmd);

//message registrations
bool OnPing(NetMessage& message, NetConnection* fromConnection);
bool OnPong(NetMessage& message, NetConnection* fromConnection);
bool OnAdd(NetMessage& message, NetConnection* fromConnection);
bool OnAddResponse(NetMessage& message, NetConnection* fromConnection);
