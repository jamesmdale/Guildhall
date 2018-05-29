#pragma once
#include "Game\Actor.hpp"
#include "Game\Map.hpp"
#include <vector>
#include "Game\UIMenu.hpp"
#include "Game\Game.hpp"
#include "Game\EncounterDefinition.hpp"
#include "Game\FloatingTextAction.hpp"
#include "Game\PauseMenu.hpp"

class PauseMenu;
class Map;
class Actor;
class FloatingTextAction;


enum EncounterState
{
	DEFAULT_ENCOUNTER_STATE,
	VICTORY_ENCOUNTER_STATE,
	TURN_ENCOUNTER_STATE,
	PAUSED_ENCOUNTER_STATE,
	NUM_ENCOUNTER_STATES
};

enum TurnState
{
	DEFAULT_TURN_STATE,
	MOVING_TURN_STATE,
	ATTACKING_TURN_STATE,
	SHOOTING_TURN_STATE,
	DEFEND_TURN_STATE,
	FIRE_TURN_STATE,
	HEAL_TURN_STATE,
	WAIT_TURN_STATE,
	NUM_TURN_STATES
};

enum TrajectoryState
{
	NO_TRAJECTORY,
	PROCESSING_TRAJECTORY,
	FINISHED_TRAJECTORY,
	NUM_TRAJECTORY_STATE
};

struct ActorInfoDisplayStruct
{
	ActorInfoDisplayStruct(std::string id, std::string details)
	{
		infoId = id;
		infoDetails = details;
	}
	std::string infoId;
	std::string infoDetails;
};

class Encounter
{
public:
	Encounter(){};
	void Initialize();
	void GenerateNewMap();	
	//void DestroyAllActors();
	void Update(float deltaSeconds);
	void Render();

	//process states
	void ProcessDefaultState(float deltaSeconds);
	void ProcessPausedState();
	void RenderPausedState();
	void ProcessTurnState(float deltaSeconds);
	void ProcessVictoryState(float deltaSeconds);
	
	//renders for states
	void RenderTurnState();
	void RenderVictoryState();	
	void RenderActorInfo(Actor* actor);

	//input updates
	void UpdateGameCamera(float deltaSeconds);
	void GameUpdateCursorFromInput(InputDirection direction);
	float UpdateMapInput(float deltaSeconds);
	float UpdateGlobalTurnInput(float deltaSeconds);

	//turn state specific updates
	void UpdateInputMovingState(float deltaSeconds);
	void UpdateInputAttackingState(float deltaSeconds);
	void UpdateInputDefaultTurnStateInput(float deltaSeconds);
	void UpdateInputFireState(float deltaSeconds);
	void UpdateInputHealState(float deltaSeconds);
	void UpdateInputDefendState(float deltaSeconds);
	void UpdateInputShootingState(float deltaSeconds);	

	void UpdateVictoryInput(float deltaSeconds);	

	//turn logic
	void CheckVictoryCondition();
	void AdvanceToNextReadyActor();
	void StartActorTurn();
	void SetCurrentStateFromSelectedMenuOption(std::string selectedMenuOptionName);

	//default input
	void UpdateDefaultInput(float deltaSeconds);

public:
	Map* m_map = nullptr;
	UIMenu* m_currentMenu = nullptr;
	Actor* m_readyActor = nullptr;

	bool m_isInCombatState = false;
	bool m_isProcessingAction = false;
	TrajectoryState m_trajectoryState = NO_TRAJECTORY;

	std::vector<FloatingTextAction*> m_pendingFloatingActions;
	EncounterDefinition* m_definition = nullptr;
};

extern PauseMenu* g_pauseMenu;
extern EncounterState g_currentEncounterState;
extern TurnState g_currentTurnState;