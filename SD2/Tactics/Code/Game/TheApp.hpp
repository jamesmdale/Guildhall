#pragma once
#include "Engine\Core\Command.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Time\Clock.hpp"

class TheApp
{
public:
	TheApp();
	~TheApp();
	void Update();
	void Render();
	void Initialize();
	void InitializeResources();
	void RunFrame();
	float UpdateFlagsFromInput(float deltaSeconds);
};

void Quit(Command &cmd);

extern TheApp* g_theApp;

