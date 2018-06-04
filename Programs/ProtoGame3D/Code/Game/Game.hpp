#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Time\Clock.hpp"

class Game
{
public:  
	Camera* m_gameCamera;
	Camera* m_uiCamera;

	bool m_isPaused = false;

public:
	Game();
	~Game();
	void Update(float TimeDelta);
	void Render();
	void Initialize();
	float UpdateFlagsFromInput(float timeDelta);
};

extern Game* g_theGame;
extern Clock* g_gameClock;






