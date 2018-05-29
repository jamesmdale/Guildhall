#pragma once

#include "Game\GameCommon.hpp"

class Game
{
public: 


public:
	Game();
	~Game();
	void Update(float TimeDelta);
	void Render();
	void Initialize();
	float UpdateFlagsFromInput(float timeDelta);
};

extern Game* g_Game;






