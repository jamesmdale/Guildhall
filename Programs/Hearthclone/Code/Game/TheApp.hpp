#pragma once
#include "Engine\Core\Command.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"

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
};

void Quit(Command &cmd);

//net tests
void Connect(Command& cmd);
void PrintLocalIP(Command& cmd);
void Disconnect(Command& cmd);

extern TheApp* g_theApp;

