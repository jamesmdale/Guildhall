#pragma once
#include <thread>
#include "Engine\Net\TCPSocket.hpp"

enum eRemoteCommandState
{
	DISCONNECTED_COMMAND_STATE,
	HOSTING_COMMAND_STATE,
	CLIENT_COMMAND_STATE
};

class RemoteCommandService
{
public:
	RemoteCommandService();
	~RemoteCommandService();

public:
	static RemoteCommandService* GetInstance();
	static  RemoteCommandService* CreateInstance();
	void Startup();

public:
	void Update();
	void Close();

private:
	bool SetupHostConnection();
	bool ConnectToHost();

	void UpdateHost();
	void CloseHost();
	void ProcessHost();

	void UpdateClient();
	void CloseClient();
	void ProcessClient();

	void UpdateDisconnected();

	void ServiceClient(TCPSocket * clientSocket);

public:
	eRemoteCommandState m_state = DISCONNECTED_COMMAND_STATE;
	std::thread* m_remoteThread = nullptr;
	TCPSocket* m_connectedSocket = nullptr;
	std::vector<TCPSocket> ConnectedClients;	
};

extern RemoteCommandService* g_theRemoteCommandService;

std::string GetLocalIP();
void ServiceClientTest(TCPSocket* clientSocket);



