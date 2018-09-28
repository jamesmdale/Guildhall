#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\NetAddress.hpp"
#include <map>
#include <string>
#include <vector>

class NetSession
{
	NetSession();
	~NetSession();

	bool BindPort(uint port, uint range);
	bool AddConnection(uint8_t connectionIndex, const NetAddress& address);

	//message processing
	void ProcessIncomingMessages();
	void ProcessOutgoingMessages();

	//message registration
	bool RegisterMessageDefinition(const std::string& name, NetMessageCallback callback);
	void LockMessageRegistration();

	NetMessageCallback GetRegisteredCallbackByName(const std::string& name);
	NetMessageCallback GetRegisteredCallbackById(int id);

public:
	UDPSocket* m_socket = nullptr;
	std::vector<NetConnection*> m_connections;

public:
	bool m_isDefinitionRegistrationLocked = false;
};

std::map<std::string, NetMessageDefinition*> s_registeredMessageDefinitions;