#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include <thread>
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\EndianHelper.hpp"
#include "Engine\Core\StringUtils.hpp"

RemoteCommandService* g_theRemoteCommandService = nullptr;
bool g_isEchoEnabled = false;
TCPSocket* g_host = nullptr;
bool g_isServiceRunning = false;
bool g_isHostRunning = false;
bool g_isClientRunning = false;


//  =========================================================================================
RemoteCommandService::RemoteCommandService()
{
	g_isServiceRunning = true;
}

//  =========================================================================================
RemoteCommandService::~RemoteCommandService()
{
	g_isServiceRunning = false;

	delete(g_host);
	g_host = nullptr;

	//delete remote thread
	if (m_remoteThread != nullptr)
	{
		delete(m_remoteThread);
		m_remoteThread = nullptr;
	}

	//remove connected client session data
	for (int connectedClientIndex = 0; connectedClientIndex < (int)m_connections.size(); ++connectedClientIndex)
	{
		m_connections[connectedClientIndex]->m_socket->CloseConnection();
		m_connections.erase(m_connections.begin() + connectedClientIndex);
	}
}

//  =========================================================================================
RemoteCommandService* RemoteCommandService::GetInstance()
{
	return g_theRemoteCommandService;
}

//  =========================================================================================
RemoteCommandService* RemoteCommandService::CreateInstance()
{
	if (g_theRemoteCommandService == nullptr)
	{
		g_theRemoteCommandService = new RemoteCommandService();
	}

	return g_theRemoteCommandService;
}

//  =============================================================================
void RemoteCommandService::Startup()
{
	//setup devconsole commands
	//register app commands
	RegisterCommand("rc", CommandRegistration(RemoteCommand, ":'idx=X command' Use to issue remote commands using", ""));
	RegisterCommand("rcb", CommandRegistration(RemoteCommandBroadcast, ":Use to issue a remote command to all connected agents", ""));
	RegisterCommand("rca", CommandRegistration(RemoteCommandAll, ":Use to issue a remote command to all connected agents and run locally", ""));
	RegisterCommand("rc_echo_toggle", CommandRegistration(RemoteCommandToggleEcho, ":Use to disable echo feedback from console", ""));
	RegisterCommand("rc_join", CommandRegistration(RemoteCommandJoin, ":Use to disconnect from current host and attempt to join a new host", ""));
	RegisterCommand("rc_host", CommandRegistration(RemoteCommandHost, ":Use to disconnect from all connections and host on the provided port", ""));
	RegisterCommand("test_message", CommandRegistration(TestBytePackerSend, "Get a string to send", ""));
	RegisterCommand("spawn_process", CommandRegistration(SpawnProcess, "Clone this process X number of times (max of 10)", ""));


	auto func = [=](){g_theRemoteCommandService->Update();};
	g_theRemoteCommandService->m_remoteThread = new std::thread(func);
	g_theRemoteCommandService->m_remoteThread->detach();
}

//  =========================================================================================
void RemoteCommandService::Shutdown()
{
	delete(g_theRemoteCommandService);
	g_theRemoteCommandService = nullptr;
}

//  =============================================================================
void RemoteCommandService::Update()
{
	while(g_isServiceRunning)
	{
		switch (m_state)
		{
		case HOSTING_COMMAND_STATE:
			UpdateHost();
			break;
		case CLIENT_COMMAND_STATE:
			UpdateClient();
			break;
		case DISCONNECTED_COMMAND_STATE:
			UpdateDisconnected();
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

//  =========================================================================================
void RemoteCommandService::Close()
{
	if(m_state == HOSTING_COMMAND_STATE)
		CloseHost();

	if(m_state == CLIENT_COMMAND_STATE)
		CloseClient();
}

//  =========================================================================================
void RemoteCommandService::SendCommand(uint index, bool isEcho, const char* messageString)
{
	BytePacker message( BIG_ENDIAN ); 
	TCPSocket* sock = GetConnectionByIndex( index ); 
	if (sock == nullptr) 
		return; 

	message.WriteBytes( sizeof(bool), &isEcho, false);  
	message.WriteString( messageString ); 

	size_t length = message.GetWrittenByteCount(); 
	ASSERT_OR_DIE( length <= 0xffff, "MESSAGE TOO LONG" ); // format doesn't support lengths larger than a max ushort; 

	uint16_t uslength = (uint16_t)length; 

	ToEndianness( &uslength, sizeof(uslength), message.GetEndianness());

	size_t lengthValue = sock->Send( 2, &uslength );  // templated overload
	size_t messageValue = sock->Send( length, message.GetBuffer() ); 
}

//  =============================================================================
void RemoteCommandService::ReceiveCommand(BytePacker* bytepacker)
{
	uint16_t sizeOfData = 0;
	bool success = bytepacker->ReadBytes(&sizeOfData, 2, false);

	bool doesEcho = true;
	success = bytepacker->ReadBytes(&doesEcho, 1, false);

	void* data = malloc(bytepacker->GetReadableByteCount() + 1);
	bytepacker->ReadString((char*)data, bytepacker->GetReadableByteCount());

	//if echo is true, print. Else run it as a command.
	if (doesEcho)
	{
		DevConsolePrintf("%s", (char*)data);
	}
	else
	{
		Command cmd = Command((char*) data);
		
		//read only the message portion of the string and not the entire buffer
		//1 byte for the bool, 2 for the size, and one for the extra appended character
		cmd.m_commandString = cmd.m_commandString.substr(0, (int)bytepacker->GetWrittenByteCount() - 4);
		
		//delete the first space after the command name
		//cmd.m_commandString.erase(cmd.m_commandString.begin());

		cmd.ParseCommandStringForValidFormatting();

		std::string index = cmd.m_commandString.substr(0, 3);
		std::string indexTest("idx");
		//they actually had an index so lets parse that.
		if (index == indexTest)
		{
			//strip index out
			CommandRun(cmd.GetRemainingContentAsString().c_str());	
		}
		else
		{
			CommandRun(cmd.m_commandString.c_str());
		}			
	}
}

//  =============================================================================
TCPSocket* RemoteCommandService::GetConnectionByIndex(int index)
{
	if(index < 0 || index > (int)m_connections.size() - 1)
		return nullptr;
	else
		return m_connections[index]->m_socket;
}

//  =============================================================================
TCPSession* RemoteCommandService::GetTCPSessionByIndex(int index)
{
	if(index < 0 || index > (int)m_connections.size() - 1)
		return nullptr;
	else		
		return m_connections[index];
}


//  =========================================================================================
bool RemoteCommandService::SetupHostConnection()
{
	g_host = new TCPSocket();	
	bool success = g_host->Listen(REMOTE_SERVICE_DEFAULT_PORT, REMOTE_SERVICE_MAX_CLIENTS);

	if (success)
	{
		g_host->SetBlockingState(NON_BLOCKING);
		g_isHostRunning = true;
	}	
	else
	{
		delete(g_host);
		g_host = nullptr;
	}

	return success;
}

//  =========================================================================================
bool RemoteCommandService::ConnectToHost()
{
	TCPSocket* connectedSocket = new TCPSocket();

	std::string addrString = GetLocalIP();
	NetAddress netAddr = NetAddress(addrString.c_str());

	bool success = connectedSocket->Connect(netAddr);	

	if (success)
	{
		TCPSession* session = new TCPSession();
		
		session->m_socket = connectedSocket;
		session->m_socket->SetBlockingState(NON_BLOCKING);
		session->m_bytePacker = new BytePacker(BIG_ENDIAN);

		m_connections.push_back(session);

		//set host socket reference
		g_host = connectedSocket; 

		g_isClientRunning = true;
	}
	else
	{
		delete(connectedSocket);
		connectedSocket = nullptr;
	}

	connectedSocket = nullptr;

	return success;
}

//HOST METHODS
//  =============================================================================
void RemoteCommandService::UpdateHost()
{
	ProcessHost();
}

//  =========================================================================================
void RemoteCommandService::CloseHost()
{
	g_host->CloseConnection();

	g_isHostRunning = false;

	//clear the data of the host
	delete(g_host);
	g_host = nullptr;

	//remove connected client session data
	for (int connectedClientIndex = 0; connectedClientIndex < (int)m_connections.size(); ++connectedClientIndex)
	{
		m_connections[connectedClientIndex]->m_socket->CloseConnection();
		m_connections.erase(m_connections.begin() + connectedClientIndex);
	}

	m_state = DISCONNECTED_COMMAND_STATE;
}

//  =========================================================================================
void RemoteCommandService::ProcessHost()
{
	//if we aren't at max capacity of connected clients, we should add any client requestingn to connect
	if (m_connections.size() < REMOTE_SERVICE_MAX_CLIENTS)
	{
		TCPSocket* client = g_host->AcceptConnection();
		if (client != nullptr)
		{
			TCPSession* newSession = new TCPSession();
			newSession->m_sessionIndex = (int)m_connections.size();
			newSession->m_socket = client;
			newSession->m_socket->SetBlockingState(NON_BLOCKING);
			newSession->m_bytePacker = new BytePacker(BIG_ENDIAN);

			m_connections.push_back(newSession);
		}

		client = nullptr;
	}

	for (int clientIndex = 0; clientIndex < (int)m_connections.size(); ++clientIndex)
	{
		ServiceClient(m_connections[clientIndex]);
	}

	//cleanup disconnected clients
	for (int clientIndex = 0; clientIndex < (int)m_connections.size(); ++clientIndex)
	{
		if (m_connections[clientIndex]->m_socket->IsClosed())
		{
			m_connections.erase(m_connections.begin() + clientIndex);
		}
	}
}

//CLIENT METHODS
//  =========================================================================================
void RemoteCommandService::UpdateClient()
{
	if (m_connections.size() > 0)
	{
		ProcessClient(m_connections[0]);
		if (m_connections[0]->m_socket->IsClosed())
		{
			m_connections.erase(m_connections.begin(), m_connections.end());
		}
	}
	else
	{
		Close();
		m_state = DISCONNECTED_COMMAND_STATE;
	}
	
}

//  =========================================================================================
void RemoteCommandService::CloseClient()
{
	g_host->CloseConnection();

	g_isHostRunning = false;
	g_isClientRunning = false;

	//clear the data of the host
	delete(g_host);
	g_host = nullptr;

	//remove connected client session data
	for (int connectedClientIndex = 0; connectedClientIndex < (int)m_connections.size(); ++connectedClientIndex)
	{
		m_connections[connectedClientIndex]->m_socket->CloseConnection();
		m_connections.erase(m_connections.begin() + connectedClientIndex);
	}

	m_state = DISCONNECTED_COMMAND_STATE;
}

//  =========================================================================================
void RemoteCommandService::ProcessClient(TCPSession* hostSession)
{
	bool isReadyToProcess = false;

	//already in big endian DO NOT CONVERT
	unsigned char buffer[256];
	hostSession->m_socket->SetBlockingState(NON_BLOCKING);

	int received = 0;

	if (hostSession->m_bytePacker->GetWrittenByteCount() < 2)
	{
		received = hostSession->m_socket->Receive(&buffer, 2);	

		if (received <= 0)
			return;

		bool success = hostSession->m_bytePacker->WriteBytes(2, (void*)&buffer, false);
		UNUSED(success);
	}

	if (hostSession->m_bytePacker->GetWrittenByteCount() >= 2)
	{
		//we've already starting reading so we need to continue reading from the last location
		size_t packetSize = hostSession->m_bytePacker->PeekBuffer(true);

		uint bytesNeeded = packetSize + 2U - hostSession->m_bytePacker->GetWrittenByteCount();

		if (bytesNeeded > 0)
		{
			received = hostSession->m_socket->Receive(&buffer, bytesNeeded);

			bool success = hostSession->m_bytePacker->WriteBytes(received, (void*)&buffer, false);
			UNUSED(success);

			bytesNeeded -= received;
		}

		if (bytesNeeded == 0)
		{
			isReadyToProcess = true;
		}
	}

	if (isReadyToProcess)
	{
		ReceiveCommand(hostSession->m_bytePacker);
		hostSession->m_bytePacker->ResetBuffer();
	}
}


//DISCONNECTED METHODS
//  =========================================================================================
void RemoteCommandService::UpdateDisconnected()
{
	//try to connect to the host
	if(ConnectToHost())
		m_state = CLIENT_COMMAND_STATE;

	//if the host doesn't exist we should try to become the host
	else if (SetupHostConnection())
		m_state = HOSTING_COMMAND_STATE;	

	//if there is a problem with us becoming the host, sleep the thread and try again.	
}

//  =========================================================================================
bool RemoteCommandService::IsCommandSystemRunning()
{
	return g_isServiceRunning;
}

//  =========================================================================================
std::string RemoteCommandService::GetHostIP()
{
	if(g_host != nullptr)
		return g_host->m_address.ToString();
	else
		return "";
}

//  =========================================================================================
void RemoteCommandService::ServiceClient(TCPSession* clientSession)
{
	bool isReadyToProcess = false;

	//already in big endian DO NOT CONVERT
	unsigned char buffer[256];
	clientSession->m_socket->SetBlockingState(NON_BLOCKING);

	int received = 0;

	if (clientSession->m_bytePacker->GetWrittenByteCount() < 2)
	{
		received = clientSession->m_socket->Receive(&buffer, 2);

		if (received <= 0)
			return;

		bool success = clientSession->m_bytePacker->WriteBytes(2, (void*)&buffer, false);
		UNUSED(success);
	}

	if (clientSession->m_bytePacker->GetWrittenByteCount() >= 2)
	{
		//we've already starting reading so we need to continue reading from the last location
		size_t packetSize = clientSession->m_bytePacker->PeekBuffer(true);

		uint bytesNeeded = packetSize + 2U - clientSession->m_bytePacker->GetWrittenByteCount();

		if (bytesNeeded > 0)
		{
			received = clientSession->m_socket->Receive(&buffer, bytesNeeded);

			bool success = clientSession->m_bytePacker->WriteBytes(received, (void*)&buffer, false);
			UNUSED(success);

			bytesNeeded -= received;
		}

		if (bytesNeeded == 0)
		{
			isReadyToProcess = true;
		}
	}

	if (isReadyToProcess)
	{
		ReceiveCommand(clientSession->m_bytePacker);
		clientSession->m_bytePacker->ResetBuffer();
	}
}

//  =============================================================================
std::string GetLocalIP()
{
	int port = REMOTE_SERVICE_DEFAULT_PORT; 

	if(g_host != nullptr)
		port = g_host->m_address.m_port;

	NetAddress netAddr;

	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(port);

	netAddr.GetMyHostAddress(&addr, &addrLength, portString.c_str());

	netAddr.FromSockAddr(&addr);

	return netAddr.ToString();
}

//  =============================================================================
std::string GetLocalIP(int port)
{
	NetAddress netAddr;

	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(port);

	netAddr.GetMyHostAddress(&addr, &addrLength, portString.c_str());

	netAddr.FromSockAddr(&addr);

	return netAddr.ToString();
}

//  =============================================================================
bool GetLocalIP(NetAddress* outAddress, int port)
{
	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(port);

	outAddress->GetMyHostAddress(&addr, &addrLength, portString.c_str());

	outAddress->FromSockAddr(&addr);

	if(outAddress->m_Ipv4Address != 0)
		return true;
	else
		return false;
}

// Dev Console Commands =============================================================================
//  =============================================================================
void RemoteCommand(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();
	eRemoteCommandState remoteState =  theCommandService->m_state;

	int connectionIndex = 0;
	std::string indexString = cmd.PeekNextString();

	std::string index = indexString.substr(0, 3);
	std::string indexTest("idx");
	//they actually had an index so lets parse that.

	cmd.ParseCommandStringForValidFormatting();
	if (index == indexTest)
	{
		//udpate the token index
		cmd.IncrementTokenIndex();

		connectionIndex = ConvertStringToInt(SplitStringOnCharacter(indexString, '=')[1]);
	}

	if(connectionIndex > (int)theCommandService->m_connections.size() - 1)
	{
		DevConsolePrintf(Rgba::RED, "INVALID CONNECTION INDEX");
		return;
	}
	
	theCommandService->SendCommand(connectionIndex, g_isEchoEnabled, cmd.m_commandTokens[cmd.m_commandTokens.size() - 1].c_str());
}

// Sends to each connection =============================================================================
void RemoteCommandBroadcast(Command& cmd)
{
	if (cmd.m_commandTokens.size() <= 1)
	{
		DevConsolePrintf(Rgba::RED, "INVALID COMMAND");
		return;
	}		

	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();
	eRemoteCommandState remoteState = theCommandService->m_state;

	std::string commandMessage = cmd.GetRemainingContentAsString();

	for (int connectionIndex = 0; connectionIndex < (int)theCommandService->m_connections.size(); ++connectionIndex)
	{
		theCommandService->SendCommand(connectionIndex, g_isEchoEnabled, commandMessage.c_str());
	}	
}

void RemoteCommandToggleEcho(Command& cmd)
{
	g_isEchoEnabled != g_isEchoEnabled;

	std::string echoState = "";

	if(g_isEchoEnabled)
		echoState = "enabled";
	else
		echoState = "disabled";

	DevConsolePrintf("Remote command echo %s!", echoState.c_str());
}

//  =========================================================================================
void RemoteCommandJoin(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();

	//close the connection and clear the state of the command service.
	theCommandService->Close();
	theCommandService->m_state = JOINING_COMMAND_STATE;

	std::string addrString = cmd.GetNextString();

	if (IsStringNullOrEmpty(addrString))
	{
		DevConsolePrintf(Rgba::RED, "Invalid input. Must be \"hostname:service_name\"");
		return;
	}
	TCPSocket* connectedSocket = new TCPSocket();

	NetAddress netAddr = NetAddress(addrString.c_str());

	bool success = connectedSocket->Connect(netAddr);

	if (success)
	{
		TCPSession* session = new TCPSession();

		session->m_socket = connectedSocket;
		session->m_socket->SetBlockingState(NON_BLOCKING);
		session->m_bytePacker = new BytePacker(BIG_ENDIAN);

		theCommandService->m_connections.push_back(session);

		//set host socket reference
		g_host = connectedSocket;

		g_isHostRunning = true;
		g_isClientRunning = true;
	}
	else
	{
		delete(connectedSocket);
		connectedSocket = nullptr;
	}

	connectedSocket = nullptr;

	if (!success)
	{
		theCommandService->m_state = DISCONNECTED_COMMAND_STATE;
		DevConsolePrintf(Rgba::RED, "Could not connect to %s", addrString.c_str());
		return;
	}
	else
	{
		theCommandService->m_state = CLIENT_COMMAND_STATE;
		DevConsolePrintf("Successfully connected to %s", netAddr.ToString().c_str());
	}

	connectedSocket = nullptr;
	theCommandService = nullptr;
}

//  =========================================================================================
void RemoteCommandHost(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();

	//close the connection and clear the state of the command service.
	theCommandService->Close();
	theCommandService->m_state = JOINING_COMMAND_STATE;


	int portNum = cmd.GetNextInt();

	if (portNum <= 0 || portNum > 65535)
	{
		DevConsolePrintf(Rgba::RED, "Invalid input. Must be \"portNum\"");
		return;
	}
	else
	{
		g_host = new TCPSocket();
		bool success = g_host->Listen(portNum, REMOTE_SERVICE_MAX_CLIENTS);

		if (success)
		{
			theCommandService->m_state = HOSTING_COMMAND_STATE;
			g_host->SetBlockingState(NON_BLOCKING);
			g_isHostRunning = true;

			DevConsolePrintf("Successfully hosting");
		}
		else
		{
			theCommandService->m_state = DISCONNECTED_COMMAND_STATE;
			delete(g_host);
			g_host = nullptr;
			DevConsolePrintf(Rgba::RED, "Could not host on port %i", portNum);
			return;
		}	
	}

	theCommandService = nullptr;
}

// Send to each connection AND run locally =============================================================================
void RemoteCommandAll(Command& cmd)
{
	if(cmd.m_commandTokens.size() <= 1)
	{
		DevConsolePrintf(Rgba::RED, "INVALID COMMAND");
		return;
	}

	RemoteCommandBroadcast(cmd);
	
	//pass the command along to the next string	
	cmd.ResetTokenIndex();
	CommandRun(cmd.GetRemainingContentAsString().c_str());
}

//  =============================================================================
void SpawnProcess(Command& cmd)
{
	int numProcesses = cmd.GetNextInt();
	if(numProcesses > 10)
		numProcesses = 10;

	for (int processIndex = 0; processIndex < numProcesses; ++processIndex)
	{
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInformation;

		ZeroMemory( &startupInfo, sizeof(startupInfo) );
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory( &processInformation, sizeof(processInformation) );

		wchar_t buffer[MAX_PATH]; 
		GetModuleFileName(NULL, buffer, MAX_PATH) ;
		CreateProcess(buffer, 0, 0, FALSE, 0, 0, 0, 0, &startupInfo, &processInformation);
	}	
}

//  =============================================================================
void HostRemoteCommandProcessor(Command& cmd, int clientIndex)
{	
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();
	eRemoteCommandState commandServiceState = theCommandService->m_state;	


	theCommandService = nullptr;
}

//  =============================================================================
void ClientRemoteCommandProcessor(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();
	eRemoteCommandState commandServiceState = theCommandService->m_state;	


	theCommandService = nullptr;
}

//  =============================================================================
void TestBytePackerSend(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();

	theCommandService->SendCommand(0, g_isEchoEnabled, "blerp");

	theCommandService = nullptr;
}


