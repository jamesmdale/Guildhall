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

//thread test
void NoThreadTest(Command& cmd);
void ThreadTest(Command& cmd);

//log with thread test
void LogThreadTest(Command& cmd);

void LogTest(const char* sourceFile, int threadCount);
void LogTestWork(void * arguments);
void ThreadTestWork(void* arguments = nullptr);

extern TheApp* g_theApp;

