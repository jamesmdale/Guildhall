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
	CLIENT_COMMAND_STATE,
	JOINING_COMMAND_STATE
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
	void Shutdown();

public:
	void Update();
	void Close();
	void SendCommand(uint index, bool isEcho, const char* messageString);
	void ReceiveCommand(BytePacker* bytePacker);

	//helpers
	TCPSocket* GetConnectionByIndex(int index);
	TCPSession* GetTCPSessionByIndex(int index);

	bool IsCommandSystemRunning();

	std::string GetHostIP();

	bool SetupHostConnection();
	bool ConnectToHost();

private:

	void UpdateHost();
	void CloseHost();
	void ProcessHost();
	void ServiceClient(TCPSession* clientSession);

	void UpdateClient();
	void CloseClient();
	void ProcessClient(TCPSession* clientSession);

	void UpdateDisconnected();

public:
	eRemoteCommandState m_state = DISCONNECTED_COMMAND_STATE;
	std::thread* m_remoteThread = nullptr;  //used for client and host

	std::vector<TCPSession*> m_connections;
};

extern RemoteCommandService* g_theRemoteCommandService;
extern bool g_isEchoEnabled;
extern bool g_isServiceRunning;

std::string GetLocalIP();
std::string GetLocalIP(int port);

bool GetLocalIP(NetAddress* outAddress, int port);

void ServiceClientTest(TCPSocket* clientSocket);

//DevConsole Commands
void RemoteCommand(Command& cmd);
void RemoteCommandAll(Command& cmd);
void RemoteCommandBroadcast(Command& cmd);
void RemoteCommandToggleEcho(Command& cmd);
void RemoteCommandJoin(Command& cmd);
void RemoteCommandHost(Command& cmd);
void SpawnProcess(Command& cmd);
void TestBytePackerSend(Command& cmd);




