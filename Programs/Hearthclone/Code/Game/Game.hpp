#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Camera\Camera.hpp"
#include <vector>
#include "Engine\Renderer\ForwardRenderingPath2D.hpp"

class Game
{
public:  
	//camera members
	Camera* m_gameCamera = nullptr;

	//rendering members
	ForwardRenderingPath2D* m_forwardRenderingPath2D = nullptr;

	//clock
	Clock* m_gameClock = nullptr;

	//game members

public:
	Game();
	~Game();
	static Game* GetInstance();
	static Game* CreateInstance();

	void Update(); //use internal clock for delta seconds;
	void PreRender();
	void Render();
	void PostRender();
	void Initialize();
	float UpdateInput(float deltaSeconds);

	//data
	//std::vector<Cards*> loadedCards;
	//std::vector<Decks*> loadedDecks;
	//std::vector<Classes*> loadedClasses;	
};






