#include "Game\Encounter.hpp"
#include "Game\Map.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Input\InputSystem.hpp"
#include <vector>
#include "Game\Actor.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
#include <cmath>
#include "Engine\Core\StringUtils.hpp"
#include "Game\MovementAction.hpp"
#include "Game\ShootingAction.hpp"


TODO("CRITICAL: Adjust this value to avoid throwing numbers at the problem");
//set camera defaults
float defaultCameraRotation = -10.f;
float defaultCameraAzimuth = -3.f;
float curVal = defaultCameraRotation;
float curVal2 = defaultCameraAzimuth;
float g_orthoZoom = 10.f;

PauseMenu* g_pauseMenu;

//input stick information
float leftStickInputCooldown = .25f;
float timeSinceLastInput = leftStickInputCooldown;
Vector2 rightStickCorrectedXY = Vector2(0.f, 0.f);
float rightStickRadius = 0.f;
Vector2 leftStickCorrectXY = Vector2(0.f, 0.f);
float leftStickRadius = 0.f;

bool isDisplayingAreaOfEffect = false;

//action variables
MovementAction* movementAction = nullptr;
ShootingAction* shootingAction = nullptr;

//map information
EncounterState g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
TurnState g_currentTurnState = DEFAULT_TURN_STATE;
ActorFaction g_winningFaction = NONE_FACTION;


void Encounter::Initialize()
{
	g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
	g_currentTurnState = DEFAULT_TURN_STATE;
	g_winningFaction = NONE_FACTION;
	g_orthoZoom = 10.f;
	
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;
	g_pauseMenu = new PauseMenu();
	gameCamera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -200.f, 200.f);
	gameCamera = nullptr;
}

void Encounter::Update(float deltaSeconds)
{
	m_isInCombatState = false;
	if(g_currentEncounterState != PAUSED_ENCOUNTER_STATE)
	{
		UpdateDefaultInput(deltaSeconds);
	}	

	switch(g_currentEncounterState)
	{
		case DEFAULT_ENCOUNTER_STATE:
			ProcessDefaultState(deltaSeconds);
			break;		
		case TURN_ENCOUNTER_STATE:
			ProcessTurnState(deltaSeconds);
			break;
		case VICTORY_ENCOUNTER_STATE:
			ProcessVictoryState(deltaSeconds);
			break;
		case PAUSED_ENCOUNTER_STATE:
			ProcessPausedState();
			break;
	}

	if((int)m_pendingFloatingActions.size() > 0)
	{
		for(int actionIndex = 0; actionIndex < (int)m_pendingFloatingActions.size(); actionIndex++)
		{
			m_pendingFloatingActions[actionIndex]->Update(deltaSeconds);
		}
	}

	m_map->Update(deltaSeconds);
}

void Encounter::Render()
{
	switch(g_currentEncounterState)
	{
	//default state has nothing to render
	case TURN_ENCOUNTER_STATE:
		RenderTurnState();
		break;
	case VICTORY_ENCOUNTER_STATE:
		RenderVictoryState();
		break;
	}

	if(g_currentEncounterState == PAUSED_ENCOUNTER_STATE)
	{
		RenderTurnState();
		RenderPausedState();
	}
		
	
}

void Encounter::ProcessDefaultState(float deltaSeconds)
{
	CheckVictoryCondition();
	if(g_winningFaction == NONE_FACTION)
	{
		AdvanceToNextReadyActor();
		StartActorTurn();
		g_currentEncounterState = TURN_ENCOUNTER_STATE;
	}
	else
	{
		g_currentEncounterState = VICTORY_ENCOUNTER_STATE;
	}
}

void Encounter::ProcessPausedState()
{
	float deltaSeconds = GetMasterClock()->GetDeltaSeconds();

	g_pauseMenu->Update(deltaSeconds);
}


void Encounter::RenderPausedState()
{
	g_pauseMenu->Render();
}

//update action states, menu, camera (FILO - reverse order in the depth of which they happen)
void Encounter::ProcessTurnState(float deltaSeconds)
{
	//make sure we enable selection for this frame unless a state says otherwise
	m_currentMenu->DisableSelection(false);
	m_map->UpdateIsCursorPositionInPathingIndices();

	switch(g_currentTurnState)
	{
	case MOVING_TURN_STATE:
		m_isInCombatState = false;
		UpdateInputMovingState(deltaSeconds);
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case ATTACKING_TURN_STATE:
		m_isInCombatState = true;
		UpdateInputAttackingState(deltaSeconds);
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case SHOOTING_TURN_STATE:
		m_isInCombatState = true;
		UpdateInputShootingState(deltaSeconds);
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case FIRE_TURN_STATE:
		m_isInCombatState = true;
		m_map->UpdateAreaOfEffectTiles(m_readyActor->m_definition->GetAbilityStructById("Fire"));
		UpdateInputFireState(deltaSeconds);		
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case HEAL_TURN_STATE:
		m_isInCombatState = true;
		UpdateInputHealState(deltaSeconds);
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case DEFEND_TURN_STATE:
		UpdateInputDefendState(deltaSeconds);
		m_currentMenu->DisableSelection(true);
		UpdateMapInput(deltaSeconds);
		break;

	case WAIT_TURN_STATE:
		m_readyActor->m_waitTime += g_waitCost;
		if(m_readyActor->m_waitTime < g_minWaitTime)
		{
			m_readyActor->m_waitTime = g_minWaitTime;
		}
		if(m_readyActor->m_waitTime > g_maxWaitTime)
		{
			m_readyActor->m_waitTime = g_maxWaitTime;
		}
		m_readyActor = nullptr;
		g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
		break;

	case DEFAULT_TURN_STATE:
		m_map->m_cursorPosition = m_map->m_currentSelectedActorPosition;
		m_map->ResetActionableTiles();
		UpdateInputDefaultTurnStateInput(deltaSeconds);
	}

	//update from inputs	
	m_currentMenu->UpdateFromInput(deltaSeconds);

	UpdateGlobalTurnInput(deltaSeconds);
	SetCurrentStateFromSelectedMenuOption(m_currentMenu->GetOptionNameIndex(m_currentMenu->m_currentSelectedOption));	
	UpdateGameCamera(deltaSeconds);
}

void Encounter::ProcessVictoryState(float deltaSeconds)
{
	UpdateVictoryInput(deltaSeconds);
}

void Encounter::RenderVictoryState()
{
	TODO("Might render map below the image but not update.  (AKA, SET GAMECLOCK TO PAUSED)");

	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	AABB2 screenBounds = AABB2(0.f, 0.f, theWindow->m_clientWidth, theWindow->m_clientHeight);
	theRenderer->SetCamera(g_theGame->m_uiCamera);

	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS, true);	
	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	const char* victoryText = "";
	Rgba victoryColor;
	switch(g_winningFaction)
	{
	case RED_ACTOR_FACTION:
			victoryText = "Red Team Wins!";
			victoryColor = Rgba::RED;
			break;
		case BLUE_ACTOR_FACTION:
			victoryColor = Rgba::BLUE;
			victoryText = "Blue Team Wins!";
	}

	theRenderer->DrawAABB(screenBounds, Rgba(0.25f, 0.25f, 0.25f, .25f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), victoryText, theWindow->m_clientHeight * .1f, victoryColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), "Press 'B' to return to main menu", theWindow->m_clientHeight * .025f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	
}


void Encounter::RenderTurnState()
{
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(gameCamera);	

	theRenderer->ClearColor(Rgba::BLACK);	
	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(LESS, true);	

	m_map->Render();	

	Actor* actor = m_map->GetActorAtCursorPosition();
	if(actor != nullptr)
	{
		RenderActorInfo(actor);
	}	

	//add turn state specifc renders here.
	switch(g_currentTurnState)
	{
		case SHOOTING_TURN_STATE:
			if(shootingAction != nullptr)
			{
				shootingAction->Render();
			}
			break;
	}

	if((int)m_pendingFloatingActions.size() > 0)
	{
		for(int actionIndex = 0; actionIndex < (int)m_pendingFloatingActions.size(); actionIndex++)
		{
			m_pendingFloatingActions[actionIndex]->Render();
		}
	}
	m_currentMenu->Render();

	theRenderer = nullptr;
	gameCamera = nullptr;
}

void Encounter::UpdateGameCamera(float deltaSeconds)
{
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;

	/*gameCamera->m_rotation += rightStickCorrectedXY.x * 3.f;
	gameCamera->m_azimuth = ClampFloat(gameCamera->m_azimuth + (rightStickCorrectedXY.y * 2), 15.f, 50.f);*/

	gameCamera->SetSphericalCoordinate(10.f, gameCamera->m_rotation, gameCamera->m_azimuth);
	Vector3 target = Vector3(m_map->m_cursorPosition.x, m_map->GetHeightForPositionXZ(m_map->m_cursorPosition), m_map->m_cursorPosition.y);

	gameCamera->LookAt(target + gameCamera->GetSphericalCoordinate(), target);
	gameCamera->SetView(gameCamera->m_cameraMatrix.InvertFastToNew());

	//cleanup gamecamera
	gameCamera = nullptr;
}

void Encounter::GameUpdateCursorFromInput(InputDirection direction)
{
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;

	Vector2 xAxis = Vector2(gameCamera->m_cameraMatrix.GetIBasis().x, gameCamera->m_cameraMatrix.GetIBasis().z);
	Vector2 zAxis = Vector2(gameCamera->m_cameraMatrix.GetKBasis().x, gameCamera->m_cameraMatrix.GetKBasis().z);

	Vector2 cameraRight = xAxis;
	Vector2 cameraLeft = -1.f * xAxis;
	Vector2 cameraForward = zAxis;
	Vector2 cameraBack = -1.f * zAxis;

	Vector2 cameraDirection;

	switch(direction)
	{
	case(FORWARD_DIRECTION):	
		cameraDirection = cameraForward;
		break;
	case(BACK_DIRECTION):	
		cameraDirection = cameraBack;
		break;
	case(LEFT_DIRECTION):
		cameraDirection = cameraLeft;
		break;
	case(RIGHT_DIRECTION):	
		cameraDirection = cameraRight;
		break;	
	}

	IntVector2 largestDot = MAP_NORTH;
	float dotValue = 0;

	float dot = DotProduct(cameraDirection, Vector2(MAP_NORTH));
	dotValue = dot;

	dot = DotProduct(cameraDirection, Vector2(MAP_SOUTH));
	if(dot > dotValue)
	{
		dotValue = dot;
		largestDot = MAP_SOUTH;
	}

	dot = DotProduct(cameraDirection, Vector2(MAP_EAST));
	if(dot > dotValue)
	{
		dotValue = dot;
		largestDot = MAP_EAST;
	}

	dot = DotProduct(cameraDirection, Vector2(MAP_WEST));
	if(dot > dotValue)
	{
		dotValue = dot;
		largestDot = MAP_WEST;
	}

	IntVector2 tempCursorPosition = m_map->m_cursorPosition + largestDot;

	if(tempCursorPosition.x >= 0 && tempCursorPosition.x < m_map->m_dimensions.x && tempCursorPosition.y >= 0 && tempCursorPosition.y < m_map->m_dimensions.z)
	{
		m_map->m_cursorPosition += largestDot;
	}	

	m_map->UpdateIsCursorPositionInPathingIndices();

	gameCamera = nullptr;
}

float Encounter::UpdateMapInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	bool isLeftStickReadyForInput = false;

	timeSinceLastInput += deltaSeconds;

	if(timeSinceLastInput > leftStickInputCooldown)
	{
		isLeftStickReadyForInput = true;
	}
	if(isLeftStickReadyForInput)
	{
		if(inputController.m_joyStickLeft.m_correctedPositionX  != 0.f && inputController.m_joyStickLeft.m_correctedPositionY != 0.f)
		{
			timeSinceLastInput = 0.f;

			leftStickCorrectXY.x = inputController.m_joyStickLeft.m_correctedPositionX;
			leftStickCorrectXY.y = inputController.m_joyStickLeft.m_correctedPositionY;		
			leftStickRadius = inputController.m_joyStickLeft.m_magnitude;

			if(leftStickRadius > .3f)
			{
				if(GetMagnitude(leftStickCorrectXY.x) > GetMagnitude(leftStickCorrectXY.y))
				{
					if(leftStickCorrectXY.x < 0.f)
					{
						GameUpdateCursorFromInput(LEFT_DIRECTION);
					}
					else
					{
						GameUpdateCursorFromInput(RIGHT_DIRECTION);
					}
				}
				else
				{
					if(leftStickCorrectXY.y < 0.f)
					{
						GameUpdateCursorFromInput(BACK_DIRECTION);
					}
					else
					{
						GameUpdateCursorFromInput(FORWARD_DIRECTION);
					}
				}
			}
		}
		else
		{
			leftStickCorrectXY = Vector2(0.f, 0.f);
			leftStickRadius = 0.f;
		}
	}	

	theInput = nullptr;
	return deltaSeconds;
}

float Encounter::UpdateGlobalTurnInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;

	if(inputController.m_joyStickRight.m_correctedPositionX  != 0 && inputController.m_joyStickRight.m_correctedPositionY != 0)
	{
		rightStickCorrectedXY.x = inputController.m_joyStickRight.m_correctedPositionX;
		rightStickCorrectedXY.y = inputController.m_joyStickRight.m_correctedPositionY;		
		rightStickRadius = inputController.m_joyStickRight.m_magnitude;
	}
	else
	{
		rightStickCorrectedXY = Vector2(0.f, 0.f);
		rightStickRadius = 0.f;
	}
	if(theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE) || inputController.WasKeyJustPressed(XBOX_BACK))
	{
		g_theGame->TransitionGameStates(MAIN_MENU_STATE);
	}
	
	if(inputController.m_rightTriggerValue > 0.1f)
	{
		g_orthoZoom -= deltaSeconds * 5.f * inputController.m_rightTriggerValue;
		if(g_orthoZoom < ORTHO_MIN)
		{
			g_orthoZoom = ORTHO_MIN;
		}

		gameCamera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -200.f, 200.f);
	}

	if(inputController.m_leftTriggerValue > 0.1f)
	{
		g_orthoZoom += deltaSeconds * 5.f * inputController.m_leftTriggerValue;
		if(g_orthoZoom > ORTHO_MAX)
		{
			g_orthoZoom = ORTHO_MAX;
		}

		gameCamera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -200.f, 200.f);
	}

	if(inputController.WasKeyJustPressed(XBOX_LEFT_SHOULDER))
	{
		float currentDegrees = gameCamera->m_rotation;

		float newAngle = 0.f;

		if(currentDegrees > 45.f && currentDegrees <= 135.f)
		{
			newAngle = 45.f;
		}
		else if(currentDegrees > 135.f && currentDegrees <= 225.f)
		{
			newAngle = 135.f;
		}
		else if(currentDegrees > 225.f && currentDegrees <= 315.f)
		{
			newAngle = 225.f;
		}
		else
		{			
			newAngle = 315.f;			
		}
		gameCamera->m_rotation = newAngle;
	}

	if(inputController.WasKeyJustPressed(XBOX_RIGHT_SHOULDER))
	{
		float currentDegrees = gameCamera->m_rotation;

		float newAngle = 0.f;

		if(currentDegrees >= 45.f && currentDegrees < 135.f)
		{
			newAngle = 135.f;
		}
		else if(currentDegrees >= 135.f && currentDegrees < 225.f)
		{
			newAngle = 225.f;
		}
		else if(currentDegrees >= 225.f && currentDegrees < 315.f)
		{
			newAngle = 315.f;
		}
		else
		{			
			newAngle = 45.f;			
		}

		gameCamera->m_rotation = newAngle;
	}

	//update camera
	gameCamera->m_rotation += rightStickCorrectedXY.x * 3.f;
	gameCamera->m_azimuth = ClampFloat(gameCamera->m_azimuth + (rightStickCorrectedXY.y * 2), 15.f, 50.f);

	gameCamera->m_rotation = std::fmod(gameCamera->m_rotation, 360.f);
	if (gameCamera->m_rotation < 0.f)
	{
		gameCamera->m_rotation += 360.f;
	}

	gameCamera = nullptr;
	theInput = nullptr;
	return deltaSeconds;
}

void Encounter::CheckVictoryCondition()
{
	int numAliveActorsRed = 0;
	int numAliveActorsBlue = 0;

	for(int actorIndex = 0;actorIndex < (int)m_map->m_actors.size(); actorIndex++)
	{
		if(m_map->m_actors[actorIndex]->m_faction == RED_ACTOR_FACTION)
		{
			numAliveActorsRed++;
		}
		if(m_map->m_actors[actorIndex]->m_faction == BLUE_ACTOR_FACTION)
		{
			numAliveActorsBlue++;
		}
	}

	if(numAliveActorsBlue == 0)
	{
		g_winningFaction = RED_ACTOR_FACTION;
	}
	else if(numAliveActorsRed == 0)
	{
		g_winningFaction = BLUE_ACTOR_FACTION;
	}
}

void Encounter::AdvanceToNextReadyActor()
{
	TODO("Later on we can maybe also check to see if the actor is dead or not");

	float shortestWaitTime = 999999;
	int indexOfActorWithShortestTime = -1;
	
	for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
	{
		if(m_map->m_actors[actorIndex]->m_waitTime < shortestWaitTime)
		{
			shortestWaitTime = m_map->m_actors[actorIndex]->m_waitTime;
			indexOfActorWithShortestTime = actorIndex;
		}
		else if(m_map->m_actors[actorIndex]->m_waitTime == shortestWaitTime) //account for tiebreakers
		{
			if(m_map->m_actors[actorIndex]->m_turnsSinceMyLastTurn > m_map->m_actors[indexOfActorWithShortestTime]->m_turnsSinceMyLastTurn)
			{
				shortestWaitTime = m_map->m_actors[actorIndex]->m_waitTime;
				indexOfActorWithShortestTime = actorIndex;
			}
		}
	}

	//smallest has been found. Now we adjust everyone's wait time and move to the turn
	for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
	{
		m_map->m_actors[actorIndex]->m_waitTime = ClampFloat(m_map->m_actors[actorIndex]->m_waitTime - (shortestWaitTime * m_map->m_actors[actorIndex]->m_definition->m_maxActionSpeed), 0.f, g_maxWaitTime);
		if(actorIndex != indexOfActorWithShortestTime)
		{
			m_map->m_actors[actorIndex]->m_turnsSinceMyLastTurn++;
		}
	}

	m_readyActor = m_map->m_actors[indexOfActorWithShortestTime];
	
	m_map->m_cursorPosition = m_readyActor->GetPositionInXZ();
	m_map->m_currentSelectedActorPosition = m_map->m_cursorPosition;
}

void Encounter::StartActorTurn()
{
	m_currentMenu = new UIMenu();
	m_readyActor->m_blockChance = 0.0f; //reset defense stance if they were in it laster turn

	int numAbilityMenuOptions = (int)m_readyActor->m_definition->m_abilityStructs.size();	

	m_currentMenu->AddMenuOption("Move", 0, false);
	
	for(int abilityIndex = 0; abilityIndex < numAbilityMenuOptions; abilityIndex++)
	{
		std::string menuOptionName = m_readyActor->m_definition->m_abilityStructs[abilityIndex].m_id;
		m_currentMenu->AddMenuOption(menuOptionName.c_str(), abilityIndex + 1, true);
	}
	m_currentMenu->AddMenuOption("Wait", numAbilityMenuOptions + 1, false);

	m_currentMenu->m_currentHoverIndex = 0;

	g_currentTurnState = DEFAULT_TURN_STATE;
	//any other startup stuff that needs to happen for the actor
}

void Encounter::GenerateNewMap()
{
	curVal = defaultCameraRotation;
	curVal2 = defaultCameraAzimuth;

	m_map = new Map();
	m_map->GenerateMap(this);
}

TODO("This is a bad method for setting states from now.  Reevaluate later about going from selected menu item to turn state");
void Encounter::SetCurrentStateFromSelectedMenuOption(std::string selectedMenuOptionName)
{
	if(selectedMenuOptionName == "Move")
	{
		if(g_currentTurnState != MOVING_TURN_STATE)
		{
			g_currentTurnState = MOVING_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, true,  m_readyActor->m_definition->m_maxTilesMovement, 1);
			return;
		}		
	}
	else if(selectedMenuOptionName == "Attack")
	{
		if(g_currentTurnState != ATTACKING_TURN_STATE)
		{
			g_currentTurnState = ATTACKING_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, false, 1, 0);
			return;
		}
	}
	else if(selectedMenuOptionName == "Shoot")
	{
		if(g_currentTurnState != SHOOTING_TURN_STATE)
		{
			AbilityStruct abilityStruct = m_readyActor->m_definition->GetAbilityStructById("Shoot");
			g_currentTurnState = SHOOTING_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, false, abilityStruct.m_range.max, abilityStruct.m_range.min);
			return;
		}
	}
	else if(selectedMenuOptionName == "Fire")
	{
		if(g_currentTurnState != FIRE_TURN_STATE)
		{
			AbilityStruct abilityStruct = m_readyActor->m_definition->GetAbilityStructById("Fire");
			g_currentTurnState = FIRE_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, false, abilityStruct.m_range.max, abilityStruct.m_range.min);
			return;
		}
	}
	else if(selectedMenuOptionName == "Heal")
	{
		if(g_currentTurnState != HEAL_TURN_STATE)
		{
			AbilityStruct abilityStruct = m_readyActor->m_definition->GetAbilityStructById("Heal");
			g_currentTurnState = HEAL_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, false, abilityStruct.m_range.max, abilityStruct.m_range.min);
			return;
		}
	}
	else if(selectedMenuOptionName == "Defend")
	{
		if(g_currentTurnState != DEFEND_TURN_STATE)
		{
			AbilityStruct abilityStruct = m_readyActor->m_definition->GetAbilityStructById("Defend");
			g_currentTurnState = DEFEND_TURN_STATE;
			m_map->ResetActionableTiles();
			m_map->SetActionableTilesForActor(m_readyActor, false, abilityStruct.m_range.max, abilityStruct.m_range.min);
			return;
		}
	}
	else if(selectedMenuOptionName == "Wait")
	{
		if(g_currentTurnState != WAIT_TURN_STATE)
		{
			m_map->ResetActionableTiles();
			g_currentTurnState = WAIT_TURN_STATE;
			return;
		}
	}	
	else
	{	
		//if none of the above are selected we are in the default state
		m_map->ResetActionableTiles();
		g_currentTurnState = DEFAULT_TURN_STATE;	
	}

	
}

void Encounter::UpdateInputMovingState(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(m_isProcessingAction)
	{
		movementAction->Update(deltaSeconds);
	}

	else if(inputController.WasKeyJustPressed(XBOX_A))
	{
		if(m_map->m_isCursorPositionInPathingIndices)
		{
			m_readyActor->GetPositionInXZ() = m_map->m_cursorPosition;
			m_map->m_currentSelectedActorPosition = m_readyActor->GetPositionInXZ();
			m_readyActor->m_waitTime += g_moveCost;
			
			m_map->m_pathTiles = m_map->GetOrderedIndexesForShortestPathInCurrentHeatMap(m_map->m_cursorPosition);

			movementAction = new MovementAction(m_map->GetOrderedIndexesForShortestPathInCurrentHeatMap(m_map->m_cursorPosition), m_readyActor, m_map, this);
			movementAction->Initialize();
		}		
	}

	theInput = nullptr;
}

void Encounter::UpdateInputAttackingState(float deltaSeconds)
{
	//currently anyone can attack anyone regardless of faction

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		//if the cursor isn't a position that we can viably attack don't bother to check.
		if(m_map->m_isCursorPositionInPathingIndices)
		{
			m_readyActor->m_animationSet->SetCurrentAnim("attack");
			for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
			{
				if(m_map->m_actors[actorIndex]->GetPositionInXZ() == m_map->m_cursorPosition)
				{
					m_map->m_actors[actorIndex]->m_animationSet->SetCurrentAnim("hit");

					AbilityStruct ability = m_readyActor->m_definition->GetAbilityStructById("Attack");
					m_readyActor->TurnTowardTargetLocation(m_map->m_cursorPosition);

					//adjust ready actor wait times
					m_readyActor->m_waitTime += ability.m_cost;
					m_currentMenu->DisableAbilityOptions();
					m_currentMenu->m_currentSelectedOption = -1;
					m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);

					FloatingTextAction* action = new FloatingTextAction((int)m_pendingFloatingActions.size(), m_map->m_actors[actorIndex], this, "", Rgba::WHITE);
					
					float blockChance =	m_map->m_actors[actorIndex]->GetCombatBlockChance(m_readyActor);
					float critChance = m_readyActor->GetCombatCritChance(m_map->m_actors[actorIndex]);
					
					bool didBlock = false;
					float calulatedBlockPercentage = GetRandomFloatInRange(0.f, 1.f);
					if(blockChance >= calulatedBlockPercentage)
					{
						didBlock = true;
					}

					float critBonus = 1.f;
					float calculatedCritPercentage = GetRandomFloatInRange(0.f, 1.f);

					if(critChance >= calculatedCritPercentage)
					{
						critBonus = 1.25f;
					}				

					bool allowBlocks = false;
					if(ability.m_type == "physical")
					{
						allowBlocks = true;
					}					

					if(allowBlocks == true && didBlock == true)
					{
						action->m_text = "BLOCKED";
					}
					else
					{						
						
						float damageValue = (ability.m_valueRange.GetRandomInRange() * critBonus);
						action->m_color = Rgba::RED;
						std::string actionText;
						if(critBonus > 1.f)
						{
							action->m_text = Stringf("%s: %i", "Crit", (int)damageValue);
						}
						else
						{
							action->m_text = Stringf("%i", (int)damageValue);
						}						

						m_map->m_actors[actorIndex]->m_health -= damageValue;									

						if(m_map->m_actors[actorIndex]->m_health <= 0)
						{
							m_map->m_actors.erase(m_map->m_actors.begin() + actorIndex);
							g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
						}
					}	

					action->Initialize();
					m_pendingFloatingActions.push_back(action);
				}
			}		
		}		
	}

	theInput = nullptr;
}


void Encounter::UpdateInputShootingState(float deltaSeconds)
{
	//currently anyone can attack anyone regardless of faction

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	AbilityStruct ability = m_readyActor->m_definition->GetAbilityStructById("Shoot");

	if(m_isProcessingAction && m_trajectoryState == PROCESSING_TRAJECTORY)
	{
		shootingAction->Update(deltaSeconds);
	}

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		//if the cursor isn't a position that we can viably attack don't bother to check.
		if(m_map->m_isCursorPositionInPathingIndices && m_trajectoryState == NO_TRAJECTORY)
		{		
			m_readyActor->TurnTowardTargetLocation(m_map->m_cursorPosition);
			
			//adjust ready actor wait times
			shootingAction = new ShootingAction(Vector3(m_map->m_cursorPosition.x, m_map->GetHeightForPositionXZ(m_map->m_cursorPosition), m_map->m_cursorPosition.y), m_readyActor, m_map, this);
			shootingAction->Initialize();					
			return;							
		}		
	}

	if(m_trajectoryState == FINISHED_TRAJECTORY)
	{
		for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
		{
			if(m_map->m_actors[actorIndex]->GetPositionInXZ() == m_map->m_cursorPosition)
			{				
				//adjust ready actor wait times
				m_readyActor->m_waitTime += ability.m_cost;
				m_currentMenu->DisableAbilityOptions();
				m_currentMenu->m_currentSelectedOption = -1;
				m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);

				m_map->m_actors[actorIndex]->m_animationSet->SetCurrentAnim("hit");

				FloatingTextAction* action = new FloatingTextAction((int)m_pendingFloatingActions.size(), m_map->m_actors[actorIndex], this, "", Rgba::WHITE);

				float blockChance =	m_map->m_actors[actorIndex]->GetCombatBlockChance(m_readyActor);
				float critChance = m_readyActor->GetCombatCritChance(m_map->m_actors[actorIndex]);

				bool didBlock = false;
				float calulatedBlockPercentage = GetRandomFloatInRange(0.f, 1.f);
				if(blockChance >= calulatedBlockPercentage)
				{
					didBlock = true;
				}

				float critBonus = 1.f;
				float calculatedCritPercentage = GetRandomFloatInRange(0.f, 1.f);

				if(critChance >= calculatedCritPercentage)
				{
					critBonus = 1.25f;
				}				

				bool allowBlocks = false;
				if(ability.m_type == "physical")
				{
					allowBlocks = true;
				}					

				if(allowBlocks == true && didBlock == true)
				{
					action->m_text = "BLOCKED";
				}
				else
				{						

					float damageValue = (ability.m_valueRange.GetRandomInRange() * critBonus);
					action->m_color = Rgba::RED;
					std::string actionText;
					if(critBonus > 1.f)
					{
						action->m_text = Stringf("%s: %i", "Crit", (int)damageValue);
					}
					else
					{
						action->m_text = Stringf("%i", (int)damageValue);
					}						

					m_map->m_actors[actorIndex]->m_health -= damageValue;									

					if(m_map->m_actors[actorIndex]->m_health <= 0)
					{
						m_map->m_actors.erase(m_map->m_actors.begin() + actorIndex);
						g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
					}
				}	

				action->Initialize();
				m_pendingFloatingActions.push_back(action);
			}
		}				

		m_trajectoryState = NO_TRAJECTORY;
	}
	

	theInput = nullptr;
}


void Encounter::UpdateInputFireState(float deltaSeconds)
{
	//currently anyone can attack anyone regardless of faction

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		//if the cursor isn't a position that we can viably attack don't bother to check.
		if(m_map->m_isCursorPositionInPathingIndices)
		{
			m_readyActor->m_animationSet->SetCurrentAnim("spell");
			m_readyActor->TurnTowardTargetLocation(m_map->m_cursorPosition);
			AbilityStruct ability = m_readyActor->m_definition->GetAbilityStructById("Fire");		
			//adjust ready actor wait times
			m_readyActor->m_waitTime += ability.m_cost;
			m_currentMenu->DisableAbilityOptions();
			m_currentMenu->m_currentSelectedOption = -1;
			m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);

			for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
			{
				m_map->m_actors[actorIndex]->m_animationSet->SetCurrentAnim("hit");
				if(m_map->GetIsPositionInsideAreaOfEffectTiles(m_map->m_actors[actorIndex]->GetPositionInXZ()) == true)
				{	
					float damageAmount = ability.m_valueRange.GetRandomInRange();
					FloatingTextAction* action = new FloatingTextAction((int)m_pendingFloatingActions.size(), m_map->m_actors[actorIndex], this, Stringf("%i", (int)damageAmount), Rgba::RED);
					
					m_map->m_actors[actorIndex]->m_health -= (damageAmount);			
					
					action->Initialize();
					m_pendingFloatingActions.push_back(action);

					if(m_map->m_actors[actorIndex]->m_health <= 0)
					{
						m_map->m_actors.erase(m_map->m_actors.begin() + actorIndex);
						g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
					}									
				}
			}

			m_map->ResetAreaOfEffectTiles();
		}		
	}

	theInput = nullptr;
}

void Encounter::UpdateInputHealState(float deltaSeconds)
{
	//currently anyone can attack anyone regardless of faction
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);
	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		//if the cursor isn't a position that we can viably attack don't bother to check.
		if(m_map->m_isCursorPositionInPathingIndices)
		{
			for(int actorIndex = 0; actorIndex < (int)m_map->m_actors.size(); actorIndex++)
			{
				if(m_map->m_actors[actorIndex]->GetPositionInXZ() == m_map->m_cursorPosition)
				{
					m_readyActor->m_animationSet->SetCurrentAnim("spell");
					m_map->m_actors[actorIndex]->m_animationSet->SetCurrentAnim("charge");

					AbilityStruct ability = m_readyActor->m_definition->GetAbilityStructById("Heal");
					m_readyActor->TurnTowardTargetLocation(m_map->m_cursorPosition);

					//adjust ready actor wait times
					m_readyActor->m_waitTime += ability.m_cost;
					m_currentMenu->DisableAbilityOptions();
					m_currentMenu->m_currentSelectedOption = -1;
					m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);
					
					float healAmount = ability.m_valueRange.GetRandomInRange();
					m_map->m_actors[actorIndex]->m_health += healAmount;

					FloatingTextAction* action = new FloatingTextAction((int)m_pendingFloatingActions.size(), m_map->m_actors[actorIndex], this, Stringf("%i", (int)healAmount), Rgba::GREEN);

					if(m_map->m_actors[actorIndex]->m_health <= 0)
					{
						m_map->m_actors.erase(m_map->m_actors.begin() + actorIndex);
						g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
					}		

					action->Initialize();
					m_pendingFloatingActions.push_back(action);
				}
			}
		}
	}

	theInput = nullptr;
}

void Encounter::UpdateInputDefendState(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);
	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		if(m_map->m_isCursorPositionInPathingIndices)
		{
			m_readyActor->m_animationSet->SetCurrentAnim("charge");
			AbilityStruct ability = m_readyActor->m_definition->GetAbilityStructById("Defend");

			//adjust ready actor wait times
			m_readyActor->m_waitTime += ability.m_cost;
			m_currentMenu->DisableAbilityOptions();
			m_currentMenu->m_currentSelectedOption = -1;
			m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);

			m_readyActor->m_blockChance = 0.5;
		}
	}
}

void Encounter::UpdateInputDefaultTurnStateInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(inputController.WasKeyJustPressed(XBOX_B))
	{
		if(m_currentMenu->m_currentSelectedOption == -1)
		{
			m_currentMenu->m_currentHoverIndex = m_currentMenu->GetOptionIndexByName("Wait");
		}
	}

	theInput = nullptr;
}


void Encounter::UpdateVictoryInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);


	if(theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE) || inputController.WasKeyJustPressed(XBOX_B))
	{
		g_theGame->TransitionGameStates(MAIN_MENU_STATE);
		g_currentTurnState = DEFAULT_TURN_STATE;
		g_currentEncounterState = DEFAULT_ENCOUNTER_STATE;
		g_winningFaction = NONE_FACTION;
	}	
}

//Render actor info

void Encounter::RenderActorInfo(Actor* actor)
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	Camera* uiCamera = g_theGame->m_uiCamera;
	AABB2 uiBounds = AABB2(theWindow->m_clientWidth * .75f, theWindow->m_clientHeight * .75f, theWindow->m_clientWidth * 0.9f, theWindow->m_clientHeight* 0.9f);
	theRenderer->SetCamera(g_theGame->m_uiCamera);

	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS, true);	
	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	//draw small menu box
	//texture from "https://colinquevitch.wordpress.com/2012/10/03/on-board/menu_background_new_002/"
	Texture* backgroundTexture = theRenderer->CreateOrGetTexture("Data/Images/uiMenuBackground.png");

	Rgba backgroundTint = Rgba::RED;
	std::string faction = "RED";
	if(actor->m_faction == BLUE_ACTOR_FACTION)
	{
		backgroundTint = Rgba::BLUE;
		faction = "BLUE";
	}
	
	//theRenderer->DrawTexturedAABB(uiBounds, *backgroundTexture, Vector2(0.f, 0.f), Vector2(1.f, 1.f), backgroundTint);	

	//need to display class, current health, current team, current wait time
	std::vector<ActorInfoDisplayStruct> infoStructs;
	infoStructs.push_back(ActorInfoDisplayStruct("Class: ", actor->m_definition->m_id));
	infoStructs.push_back(ActorInfoDisplayStruct("Health: ", std::to_string(actor->m_health)));
	infoStructs.push_back(ActorInfoDisplayStruct("Faction: ", faction));
	infoStructs.push_back(ActorInfoDisplayStruct("Wait Time: ", std::to_string((int)actor->m_waitTime)));
	//TrimStringToDecimalPlace()

	//calculate for text in box
	Vector2 uiDimensions = uiBounds.GetDimensions();	

	int longestStringAmount = 0;
	for(int infoIndex = 0; infoIndex < (int)infoStructs.size(); infoIndex++)
	{
		int infoSize = (int)infoStructs[infoIndex].infoDetails.size() + (int)infoStructs[infoIndex].infoId.size();

		if(infoSize > longestStringAmount)
		{
			longestStringAmount = infoSize;
		}
	}

	float offsetAmountX = uiDimensions.x/longestStringAmount + 1.f;
	float offsetAmountY = uiDimensions.y / ((int)infoStructs.size() + 1.f);
	float scale = offsetAmountX/offsetAmountY;

	Vector2 uiBoxTopCenter = Vector2(uiBounds.GetCenter().x, uiBounds.GetCenter().y + (uiDimensions.y * .5f));	
	
	for(int optionIndex = 0; optionIndex < (int)infoStructs.size(); optionIndex++)
	{
		Rgba textColor = Rgba::WHITE;

		Vector2 drawTextAtPosition = Vector2(uiBoxTopCenter.x, uiBoxTopCenter.y - (offsetAmountY * ((float)optionIndex + 1)));

		std::string printString = Stringf("%s%s", infoStructs[optionIndex].infoId.c_str(), infoStructs[optionIndex].infoDetails.c_str());
		

		theRenderer->DrawText2DCentered(drawTextAtPosition, printString, offsetAmountY, textColor, scale, theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	}

	//clean up pointers
	uiCamera = nullptr;
	theRenderer = nullptr;
	theWindow = nullptr;	
}

void Encounter::UpdateDefaultInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(theInput->WasKeyJustPressed(theInput->KEYBOARD_F10) || inputController.WasKeyJustPressed(XBOX_START))
	{
		g_theGame->TransitionEncounterStates(PAUSED_ENCOUNTER_STATE);
		g_gameClock->SetPaused(true);
		g_pauseMenu->ResetMenuState();
	}

	theInput = nullptr;
}