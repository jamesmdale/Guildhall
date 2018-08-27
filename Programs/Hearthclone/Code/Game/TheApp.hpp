#pragma once
#include "Engine\Core\Command.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include <thread>

class TheApp
{
public:
	TheApp();
	~TheApp();
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void Initialize();
	void RunFrame();
	float UpdateInput(float timeDelta);

	std::thread* m_hostThread = nullptr;
};

void Quit(Command &cmd);

//tests for network
bool StartTestServer(uint port);
void ProcessHost(void*);
void CloseHost();
void ServiceClient(TCPSocket* clientSocket);

void ConnectAndSend(Command& cmd);
void PrintLocalIP(Command& cmd);
void Disconnect(Command& cmd);
std::string GetLocalIP();
void HostConnection(Command& cmd);

extern TheApp* g_theApp;
extern TCPSocket* host;


