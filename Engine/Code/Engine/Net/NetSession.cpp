#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include <string>

NetSession* g_theNetSession = nullptr;


//  =============================================================================
NetSession::NetSession()
{
}

//  =============================================================================
NetSession::~NetSession()
{
	delete(m_socket);
m_socket = nullptr;

for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
{
	//disconnect m_connections[connectionIndex]
	m_connections[connectionIndex]->FlushOutgoingMessages();
	delete(m_connections[connectionIndex]);
	m_connections[connectionIndex] = nullptr;
}
}

//  =============================================================================
NetSession* NetSession::GetInstance()
{
	return g_theNetSession;
}

//  =============================================================================
NetSession* NetSession::CreateInstance()
{
	if (g_theNetSession == nullptr)
	{
		g_theNetSession = new NetSession();
	}
	return g_theNetSession;
}

//  =========================================================================================
void NetSession::Startup()
{
	//register messages

}

//  =============================================================================
bool NetSession::BindPort(uint port, uint range)
{
	m_socket = new UDPSocket();

	NetAddress address;

	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(port);

	address.GetMyHostAddress(&addr, &addrLength, portString.c_str());
	address.FromSockAddr(&addr);

	return m_socket->Bind(address, range);
}

//  =============================================================================
bool NetSession::AddConnection(uint8_t connectionIndex, const NetAddress & address)
{
	NetConnection* connection = new NetConnection();
	connection->m_index = connectionIndex;
	connection->m_address = address;

	m_connections.push_back(connection);
}

//  =============================================================================
void NetSession::ProcessIncomingMessages()
{
	//read from socket until I can't read anymore
}

//  =============================================================================
void NetSession::ProcessOutgoingMessages()
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		m_connections[connectionIndex]->FlushOutgoingMessages();
	}
}

//  =============================================================================
bool NetSession::RegisterMessageDefinition(const std::string& name, NetMessageCallback callback)
{
	bool success = false;

	if (!m_isDefinitionRegistrationLocked)
	{
		NetMessageDefinition* definition = new NetMessageDefinition();
		definition->m_callbackName = name;
		definition->m_callback = callback;

		s_registeredMessageDefinitions.insert(std::pair<std::string, NetMessageDefinition*>(name, definition));
		success = true;
	}

	return success;
}

//  =========================================================================================
void NetSession::LockMessageDefinitionRegistration()
{
	m_isDefinitionRegistrationLocked = true;

	int idCount = 0;
	for (std::map<std::string, NetMessageDefinition*>::iterator defIterator = s_registeredMessageDefinitions.begin(); defIterator != s_registeredMessageDefinitions.end(); ++defIterator)
	{
		defIterator->second->m_callbackId = idCount;
		++idCount;
	}
}

//  =========================================================================================
//  Callback
//  =========================================================================================
NetMessageCallback GetRegisteredCallbackByName(const std::string& name)
{
	NetMessageDefinition* definition =  s_registeredMessageDefinitions.find(name)->second;
	return definition->m_callback;
}

//  =========================================================================================
NetMessageCallback GetRegisteredNetCallbackById(int id)
{
	return s_registeredMessageDefinitions.begin() + id;
}

//  =========================================================================================
//  Console Commands
//  =========================================================================================
void AddConnection(Command & cmd)
{
	//uint8_t index;
	//NetAddress address;

	int index = cmd.GetNextInt();
	std::string address = cmd.GetNextString();

	if (IsStringNullOrEmpty(address))
		DevConsolePrintf("Address is invalid");
}

//  =========================================================================================
void SendPing(Command & cmd)
{
}

//  =========================================================================================
void SendAdd(Command & cmd)
{
}


