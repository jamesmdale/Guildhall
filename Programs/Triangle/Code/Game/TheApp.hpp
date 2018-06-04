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
	void Render();
	void Initialize();
	void RunFrame();
	float UpdateFlagsFromInput(float timeDelta);
};

void Quit(Command &cmd);

extern TheApp* g_theApp;

