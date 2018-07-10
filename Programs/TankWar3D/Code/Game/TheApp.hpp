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
	float UpdateInput(float deltaSeconds);
};

void Quit(Command &cmd);
void NoThreadTest(Command &cmd);
void ThreadTest(Command &cmd);

void ThreadTestWork(void* arguments = nullptr);

extern TheApp* g_theApp;

