#include "Engine\Net\NetSession.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"



NetSession::NetSession()
{
}

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

bool NetSession::AddConnection(uint8_t connectionIndex, const NetAddress & address)
{
	NetConnection* connection = new NetConnection();
	connection->m_index = connectionIndex;
	connection->m_address = address;

	m_connections.push_back(connection);
}

void NetSession::ProcessIncomingMessages()
{
	//read from socket until I can't read anymore
}

void NetSession::ProcessOutgoingMessages()
{
	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
	{
		m_connections[connectionIndex]->FlushOutgoingMessages();
	}
}

bool NetSession::RegisterMessageDefinition(const std::string& name, NetMessageCallback callback)
{
	bool success = false;

	if (!m_isDefinitionRegistrationLocked)
	{
		NetMessageDefinition* definition = new NetMessageDefinition();
		definition->m_callbackName = name;
		definition->m_callback = callback;
		//definition->m_callbackId = (int)s_registeredMessageDefinitions.size();

		s_registeredMessageDefinitions.insert(std::pair<std::string, NetMessageDefinition*>(name, definition));
		success = true;
	}
		
	return success;
}

void NetSession::LockMessageRegistration()
{
	m_isDefinitionRegistrationLocked = true;
}
