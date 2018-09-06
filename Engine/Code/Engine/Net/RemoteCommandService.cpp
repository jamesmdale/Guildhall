#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include <thread>

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
	if (m_remoteThread != nullptr)
	{
		m_remoteThread->join();
		delete(m_remoteThread);
		m_remoteThread = nullptr;
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
bool RemoteCommandService::SetupHostConnection()
{
	host = new TCPSocket();
	bool success = host->Listen(REMOTE_SERVICE_DEFAULT_PORT, REMOTE_SERVICE_MAX_CLIENTS);

	if (success)
		g_isHostRunning = true;

	return success;
}

//  =========================================================================================
bool RemoteCommandService::ConnectToHost()
{
	TCPSocket socket;

	std::string addrString = GetLocalIP();
	NetAddress netAddr = NetAddress(addrString.c_str());

	bool success = socket.Connect(netAddr);	
	return success;
}

//HOST METHODS
//  =============================================================================
void RemoteCommandService::UpdateHost()
{
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
	else
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	
}

//  =========================================================================================
void RemoteCommandService::ServiceClient(TCPSocket* clientSocket)
{
	/*unsigned char buffer[256];
	int received = clientSocket->Receive(&buffer, 256 - 1U);
	if (received > 0) 
	{
		buffer[received] = NULL;
		DevConsolePrintf("Receieved: %s", buffer);

		std::string response = "gotcha";
		clientSocket->Send((void*)&response);
	}

	clientSocket->CloseConnection();*/
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
