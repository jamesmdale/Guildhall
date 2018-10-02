#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\Command.hpp"
#include <map>
#include <string>
#include <vector>

class NetSession
{
public:
	NetSession();
	~NetSession();
	static NetSession* GetInstance();
	static NetSession* CreateInstance();

	void Startup();

	bool BindPort(uint port, uint range);
	bool AddConnection(uint8_t connectionIndex, NetAddress* address);

	//message processing
	void ProcessIncomingMessages();
	void ProcessOutgoingMessages();

	//message registration
	bool RegisterMessageDefinition(const std::string& name, NetMessageCallback callback);
	void LockMessageDefinitionRegistration();

	NetConnection* GetConnectionById(uint8_t id);

public:
	UDPSocket* m_socket = nullptr;
	std::vector<NetConnection*> m_connections;

public:
	bool m_isDefinitionRegistrationLocked = false;
};

//net callbacks
NetMessageCallback GetRegisteredNetCallbackById(int id);
NetMessageCallback GetRegisteredCallbackByName(const std::string& name);
NetMessageDefinition* GetRgisteredDefinitionById(int id);
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string& name);

static std::map<std::string, NetMessageDefinition*> s_registeredMessageDefinitions;

//console commands
void AddConnection(Command& cmd);
void SendPing(Command& cmd);
void SendAdd(Command& cmd);