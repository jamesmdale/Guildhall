#pragma once
#include <thread>
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\TCPSession.hpp"
#include "Engine\Core\Command.hpp"

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
	void SendCommand(uint index, bool isEcho, const char* messageString);
	void ReceiveCommand(void* data, size_t bufferSize);

	//helpers
	TCPSocket* GetConnectionByIndex(int index);
	TCPSession* GetTCPSessionByIndex(int index);

private:
	bool SetupHostConnection();
	bool ConnectToHost();

	void UpdateHost();
	void CloseHost();
	void ProcessHost();
	void ServiceClient(TCPSession* clientSession);

	void UpdateClient();
	void CloseClient();
	void ProcessClient();

	void UpdateDisconnected();

public:
	eRemoteCommandState m_state = DISCONNECTED_COMMAND_STATE;
	std::thread* m_remoteThread = nullptr;  //used for client and host

	//If hosting, this is the listening socket. If client this is the reading/writing socket
	TCPSocket* m_connectedSocket = nullptr;

	//Host only
	std::vector<TCPSession*> m_connectedClients;

	//Client only
	BytePacker* m_bytePacker = nullptr;

};

extern RemoteCommandService* g_theRemoteCommandService;

std::string GetLocalIP();
void ServiceClientTest(TCPSocket* clientSocket);

//DevConsole Commands
void TestBytePackerSend(Command& cmd);
void RemoteCommand(Command& cmd);
void RemoteCommandAll(Command& cmd);
void RemoteCommandBroadcast(Command& cmd);
void HostRemoteCommandProcessor(Command& cmd, int clientIndex);
void ClientRemoteCommandProcessor(Command& cmd);



