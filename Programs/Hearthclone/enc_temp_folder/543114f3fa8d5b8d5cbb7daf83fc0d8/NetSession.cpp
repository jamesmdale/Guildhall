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
bool NetSession::AddConnection(uint8_t connectionIndex, NetAddress* address)
{
	if (m_connections.size() > connectionIndex)
	{
		if(m_connections[connectionIndex] != nullptr)
			return false;
	}
	else if(m_connections.size() < connectionIndex)
	{
		return false;
	}

	NetConnection* connection = new NetConnection();
	connection->m_index = connectionIndex;
	connection->m_address = address;	

	m_connections.push_back(connection);
	return true;
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
NetConnection* NetSession::GetConnectionById(uint8_t id)
{
	if (id < m_connections.size() )
	{
		return m_connections[(int)id];
	}

	return nullptr;	
}

//  =========================================================================================
//  Callback
//  =========================================================================================

NetMessageDefinition* GetRgisteredDefinitionById(int id)
{
	std::map<std::string, NetMessageDefinition*>::iterator definitionIterator;
	std::advance(definitionIterator, id);
	if (definitionIterator != s_registeredMessageDefinitions.end())
	{
		return definitionIterator->second;
	}

	return nullptr;
}

//  =========================================================================================
NetMessageDefinition* GetRegisteredDefinitionByName(const std::string & name)
{
	std::map<std::string, NetMessageDefinition*>::iterator definitionIterator;
	definitionIterator = s_registeredMessageDefinitions.find(name);
	if (definitionIterator != s_registeredMessageDefinitions.end())
	{
		return definitionIterator->second;
	}

	return nullptr;
}

//  =========================================================================================
NetMessageCallback GetRegisteredCallbackByName(const std::string& name)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionByName(name);
	if(definition != nullptr)
		return definition->m_callback;

	return nullptr;
}

//  =========================================================================================
NetMessageCallback GetRegisteredNetCallbackById(int id)
{
	NetMessageDefinition* definition = GetRgisteredDefinitionById(id);
	if (definition != nullptr)
		return definition->m_callback;

	return nullptr;	
}

//  =========================================================================================
//  Console Commands
//  =========================================================================================
void AddConnection(Command& cmd)
{
	int index = cmd.GetNextInt();
	std::string address = cmd.GetNextString();

	NetSession* theNetSession = NetSession::GetInstance();

	if (theNetSession->m_connections.size() != index)
	{
		DevConsolePrintf(Rgba::RED, "Requested index %i is invalid", index);
	}

	if (IsStringNullOrEmpty(address))
		DevConsolePrintf(Rgba::RED, "Address is invalid");

	NetAddress* netAddress = new NetAddress(address.c_str());

	NetSession::GetInstance()->AddConnection(index, netAddress);

	DevConsolePrintf("Successfully added address (%s) at index %i", address.c_str(), index);

	//cleanup
	netAddress = nullptr;
	theNetSession = nullptr;
}

//  =========================================================================================
void SendPing(Command& cmd)
{
	uint8_t index = (uint8_t)cmd.GetNextInt();

	NetSession* theNetSession = NetSession::GetInstance();
	//NetConnection* connection = theNetSession->GetConnection(index);

	theNetSession = nullptr;
}

//  =========================================================================================
void SendAdd(Command& cmd)
{
	int index = cmd.GetNextInt();

	float parameter1 = cmd.GetNextFloat();
	float parameter2 = cmd.GetNextFloat();

	if (index == INT_MAX) {
		DevConsolePrintf(Rgba::RED, "Must provide an index." ); 
		return; 
	}
	NetSession* theNetSession = NetSession::GetInstance();
	
	NetConnection* connection = theNetSession->GetConnectionById((uint8_t)index);
	if (connection == nullptr) 
	{
		DevConsolePrintf( "No connection at index %u", index ); 
		return; 
	}

	NetMessage* message = new NetMessage("ping");
	//char const *str = args.get_next_string();
	//msg.write_string( str ); 

	// messages are sent to connections (not sessions)
	connection->QueueMessage( message ); 

	message = nullptr;
	theNetSession = nullptr;
}


