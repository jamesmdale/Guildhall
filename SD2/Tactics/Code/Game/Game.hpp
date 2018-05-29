#pragma once

#include "Game\GameCommon.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include <string.h>
#include "Game\EncounterDefinition.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Time\Clock.hpp"

enum InputDirection
{
	FORWARD_DIRECTION,
	BACK_DIRECTION,
	RIGHT_DIRECTION,
	LEFT_DIRECTION,
	NUM_INPUT_DIRECTIONS
};

class Game
{
public: 
	OrbitCamera* m_gameCamera;
	Camera* m_uiCamera;

	bool m_isPaused = false;

public:
	Game();
	~Game();
	void Update(float deltaSeconds);
	void TransitionGameStates(GameState toState);
	void TransitionEncounterStates(int toState);
	void Render();
	void Initialize();

	//update functions
	void UpdateMainMenu(float deltaSeconds);
	void UpdateLoading(float deltaSeconds);
	void UpdateGame(float deltaSeconds);

	//rendering functions
	void RenderMainMenu();
	void RenderLoading();
	void RenderGame();

	//Encounter Instantiation and Commands
	void Game::RegisterGameCommands();	
	
	
};

extern Game* g_theGame;
extern Clock* g_gameClock;
void InstantiateNewEncounter(Command & cmd);
void CreateAndSetNewEncounterByDefinition(EncounterDefinition * encounterDef);
EncounterDefinition* FindEncounterByName(const std::string & encounterName);



