#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include <thread>
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\EndianHelper.hpp"

RemoteCommandService* g_theRemoteCommandService = nullptr;
TCPSocket* host = nullptr;
bool g_isServiceRunning = false;
bool g_isHostRunning = false;

//  =========================================================================================
RemoteCommandService::RemoteCommandService()
{
	g_isServiceRunning = true;
}

//  =========================================================================================
RemoteCommandService::~RemoteCommandService()
{
	g_isServiceRunning = false;

	//delete remote thread
	if (m_remoteThread != nullptr)
	{
		m_remoteThread->join();
		delete(m_remoteThread);
		m_remoteThread = nullptr;
	}

	//remove connected client session data
	for (int connectedClientIndex = 0; connectedClientIndex < (int)m_connectedClients.size(); ++connectedClientIndex)
	{
		delete(m_connectedClients[connectedClientIndex]);
		m_connectedClients[connectedClientIndex] = nullptr;
	}

	//remove bytepacker data
	if (m_bytePacker != nullptr)
	{
		delete(m_bytePacker);
		m_bytePacker = nullptr;
	}

	//remove connected socket
	if (m_connectedSocket != nullptr)
	{
		m_connectedSocket->CloseConnection();
		delete(m_connectedSocket);
		m_connectedSocket = nullptr;
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
	CommandRegister("rc", CommandRegistration(RemoteCommand, ": 'idx=X command' Use to issue remote commands using", ""));
	CommandRegister("rcb", CommandRegistration(RemoteCommandBroadcast, ": Use to issue a remote command to all connected agents", ""));
	CommandRegister("rca", CommandRegistration(RemoteCommandAll, ":Use to issue a remote command to all connected agents and run locally", ""));
	CommandRegister("rc_echo", CommandRegistration(RemoteCommandAll, ":Use to disable echo feedback from console", ""));
	CommandRegister("rc_join", CommandRegistration(RemoteCommandAll, ":Use to disconnect from current host and attempt to join a new host", ""));
	CommandRegister("rc_host", CommandRegistration(RemoteCommandAll, ":Use to disconnect from all connections and host on the provided port", ""));


	auto func = [=](){g_theRemoteCommandService->Update();};
	g_theRemoteCommandService->m_remoteThread = new std::thread(func);
	g_theRemoteCommandService->m_remoteThread->detach();
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

	bool doesEcho = false;
	success = bytepacker->ReadBytes(&doesEcho, 1, false);

	void* data = malloc(bytepacker->GetReadableByteCount() + 1);
	bytepacker->ReadString((char*)data, bytepacker->GetReadableByteCount());

	//if echo is true, print. Else run it as a command.
	
	//testing for now just print.
	DevConsolePrintf("%s", (char*)data);	


	//m_bytePacker
}

//  =============================================================================
TCPSocket* RemoteCommandService::GetConnectionByIndex(int index)
{
	if(index < 0 || index > (int)m_connectedClients.size() - 1)
		return nullptr;
	else
		return m_connectedClients[index]->m_socket;
}

//  =============================================================================
TCPSession* RemoteCommandService::GetTCPSessionByIndex(int index)
{
	if(index < 0 || index > (int)m_connectedClients.size() - 1)
		return nullptr;
	else		
		return m_connectedClients[index];
}


//  =========================================================================================
bool RemoteCommandService::SetupHostConnection()
{
	host = new TCPSocket();	
	bool success = host->Listen(REMOTE_SERVICE_DEFAULT_PORT, REMOTE_SERVICE_MAX_CLIENTS);

	if (success)
	{
		host->SetBlockingState(NON_BLOCKING);
		g_isHostRunning = true;
	}	
	else
	{
		delete(host);
		host = nullptr;
	}

	return success;
}

//  =========================================================================================
bool RemoteCommandService::ConnectToHost()
{
	//confirm data is nullptr
	m_bytePacker = nullptr;
	m_connectedSocket = nullptr;

	m_connectedSocket = new TCPSocket();

	std::string addrString = GetLocalIP();
	NetAddress netAddr = NetAddress(addrString.c_str());

	bool success = m_connectedSocket->Connect(netAddr);	

	if (success)
	{
		m_bytePacker = new BytePacker(BIG_ENDIAN);
		m_connectedSocket->SetBlockingState(NON_BLOCKING);
	}
	else
	{
		delete(host);
		host = nullptr;
	}


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
	host->CloseConnection();

	g_isHostRunning = false;

	delete(host);
	host = nullptr;
}

//  =========================================================================================
void RemoteCommandService::ProcessHost()
{
	//if we aren't at max capacity of connected clients, we should add any client requestingn to connect
	if (m_connectedClients.size() < REMOTE_SERVICE_MAX_CLIENTS)
	{
		TCPSocket* client = host->AcceptConnection();
		if (client != nullptr)
		{
			TCPSession* newSession = new TCPSession();
			newSession->m_sessionIndex = (int)m_connectedClients.size();
			newSession->m_socket = client;
			newSession->m_socket->SetBlockingState(NON_BLOCKING);
			newSession->m_bytePacker = new BytePacker(BIG_ENDIAN);

			m_connectedClients.push_back(newSession);
		}

		client = nullptr;
	}

	for (int clientIndex = 0; clientIndex < (int)m_connectedClients.size(); ++clientIndex)
	{
		ServiceClient(m_connectedClients[clientIndex]);
	}
}

//CLIENT METHODS
//  =========================================================================================
void RemoteCommandService::UpdateClient()
{
}

//  =========================================================================================
void RemoteCommandService::CloseClient()
{
}

//  =========================================================================================
void RemoteCommandService::ProcessClient()
{
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
void RemoteCommandService::ServiceClient(TCPSession* clientSession)
{
	bool isReadyToProcess = false;

	//already in big endian DO NOT CONVERT
	unsigned char buffer[256];
	clientSession->m_socket->SetBlockingState(NON_BLOCKING);
	int received = clientSession->m_socket->Receive(&buffer, REMOTE_SERVICE_MAX_BUFFER_SIZE);

	if (received > 0) 
	{
		BytePacker* clientBytepacker = clientSession->m_bytePacker;

		if (!TCPSocket::HasFatalError())
		{		
			//we aren't currently reading anything so the first byte we can assume is the size
			if (clientSession->m_bytePacker->GetWrittenByteCount() == 0)
			{
				//writebytes needs to NOT CONVERT

				//get the size and write that in
				bool success = clientSession->m_bytePacker->WriteBytes(2, (void*)&buffer, false);
				
				success = clientSession->m_bytePacker->WriteBytes(received - 2, (void*)&buffer, false);

				size_t packetSize = clientSession->m_bytePacker->PeekBuffer(true);

				if (clientSession->m_bytePacker->GetWrittenByteCount() == packetSize) //|| clientSession->m_bytePacker->GetWrittenByteCount() == REMOTE_SERVICE_MAX_BUFFER_SIZE)
				{
					isReadyToProcess = true;
				}
			}
			else
			{
				//we've already starting reading so we need to continue reading from the last location
				size_t amountToWrite = 0;

			/*	if(received + clientSession->m_bytePacker->GetWrittenByteCount() > REMOTE_SERVICE_MAX_BUFFER_SIZE)
					amountToWrite = REMOTE_SERVICE_MAX_BUFFER_SIZE;
				else
					amountToWrite = received;*/

				bool success = clientSession->m_bytePacker->WriteBytes(received, (void*)&buffer, false);

				size_t packetSize = clientSession->m_bytePacker->PeekBuffer(true);
				if (clientSession->m_bytePacker->GetWrittenByteCount() == packetSize)// || clientSession->m_bytePacker->GetWrittenByteCount())
				{
					isReadyToProcess = true;
				}
								
			}

			// if we are ready to process the command (ie read the size to the maxsize of a single buffer or the entire message),
			// process the command.
			if (isReadyToProcess)
			{
				ReceiveCommand(clientSession->m_bytePacker);

				clientSession->m_bytePacker->ResetBuffer();
			}				

			//buffer[received] = NULL;
			//DevConsolePrintf("Receieved: %s", buffer);

			//std::string response = "gotcha";
			//clientSession->m_socket->Send((void*)&response);
		}	
		else
		{
			clientSession->m_socket->CloseConnection();
			m_connectedClients.erase(m_connectedClients.begin() + clientSession->m_sessionIndex);
		}
	}
}

//  =============================================================================
std::string GetLocalIP()
{
	NetAddress netAddr;

	sockaddr addr;
	int addrLength = 0;
	std::string portString = std::to_string(REMOTE_SERVICE_DEFAULT_PORT);

	netAddr.GetMyHostAddress(&addr, &addrLength, portString.c_str());

	netAddr.FromSockAddr(&addr);

	return netAddr.ToString();
}


// Dev Console Commands =============================================================================
//  =============================================================================
void RemoteCommand(Command& cmd)
{
	cmd.GetNextString();

	//idx=0;

	cmd.GetNextString();

	//echo = false

	//
}

// Sends to each connection =============================================================================
void RemoteCommandBroadcast(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();	

	if (theCommandService->m_state == HOSTING_COMMAND_STATE)
	{
		for (int connectedClientIndex = 0; connectedClientIndex < (int)theCommandService->m_connectedClients.size(); ++connectedClientIndex)
		{
			HostRemoteCommandProcessor(cmd, connectedClientIndex);
		}
	}
	else
	{
		ClientRemoteCommandProcessor(cmd);
	}

	theCommandService = nullptr;
}

// Send to each connection AND run locally =============================================================================
void RemoteCommandAll(Command& cmd)
{
	RemoteCommandBroadcast(cmd);
	
	//pass the command along to the next string
	CommandRun(cmd.m_commandString.c_str());
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