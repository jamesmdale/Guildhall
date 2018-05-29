#include "Game.hpp"
#include <stdlib.h>
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\Image.hpp"
#include "Game\Map.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Menu.hpp"
#include "Game\Encounter.hpp"
#include "Engine\Math\Trajectory.hpp"

Game* g_theGame = nullptr;
Menu* g_theMenu = nullptr;
Clock* g_gameClock = nullptr;
Encounter* g_theEncounter = nullptr;

GameState g_currentState;
GameState g_transitionState;
float g_secondsInState;
float g_secondsTransistioning;
bool g_isFinishedTransistioning;

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::Initialize()
{	
	TODO("Set game to start at main menu. For now. Make the state game.");
	g_currentState = MAIN_MENU_STATE;
	g_transitionState = NONE_STATE;

	//initialize game camera
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	m_gameCamera = new OrbitCamera();
	m_gameCamera->SetDepthStencilTarget(theRenderer->GetDefaultDepthStencilTarget());
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());	
	m_gameCamera->SetProjectionOrtho(10.f, CLIENT_ASPECT, -200.f, 200.f);
	m_gameCamera->m_azimuth = 30.f; //set default roation 

	//initialize UI camera
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_uiCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_uiCamera->SetView(Matrix44::IDENTITY);

	theRenderer = nullptr;
	theWindow = nullptr;

	//initialize menu
	g_theMenu = new Menu();

	g_gameClock = new Clock(GetMasterClock());

	RegisterGameCommands();
}

void Game::Update(float deltaSeconds)
{	
	if(g_transitionState != NONE_STATE)
	{
		if(g_transitionState == PLAYING_STATE)
		{
			EncounterDefinition* def = FindEncounterByName("standard");

			GUARANTEE_OR_DIE(def != nullptr, "Standard encounter definition not properly loaded");
			CreateAndSetNewEncounterByDefinition(def);
		}
		g_currentState = g_transitionState;
		g_transitionState = NONE_STATE;
		g_secondsInState = 0.f;
	}

	switch(g_currentState)
	{
	case MAIN_MENU_STATE:
		UpdateMainMenu(deltaSeconds);
		break;
	case LOADING_STATE:
		UpdateLoading(deltaSeconds);
		break;
	case PLAYING_STATE:
		UpdateGame(deltaSeconds);
		break;
	}

	g_secondsInState += deltaSeconds;
}

void Game::TransitionGameStates(GameState toState)
{
	g_transitionState = toState;
}

void Game::TransitionEncounterStates(int toState)
{
	g_currentEncounterState = (EncounterState)toState;
}

void Game::Render()
{
	switch(g_currentState)
	{
	case MAIN_MENU_STATE:
		RenderMainMenu();
		break;
	case LOADING_STATE:
		RenderLoading();
		break;
	case PLAYING_STATE:
		RenderGame();
		break;
	}
}


//Update & Render for MAIN_MENU state
void Game::UpdateMainMenu(float deltaSeconds)
{
	g_theMenu->Update(deltaSeconds);
}

void Game::RenderMainMenu()
{
	g_theMenu->Render();
}


//update & render for LOADING state
void Game::UpdateLoading(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

TODO("Complete loading rendering");
void Game::RenderLoading()
{

}

//update & render for GAME state
void Game::UpdateGame(float deltaSeconds)
{
	g_theEncounter->Update(deltaSeconds);	
}

void Game::RenderGame()
{	
	g_theEncounter->Render();
};

void Game::RegisterGameCommands()
{
	CommandRegister("encounter", CommandRegistration(InstantiateNewEncounter, ": Type a single string to search the encounter definition list for a def of that name", "Instantiating new encounter..."));
}

void InstantiateNewEncounter(Command &cmd)
{
	if(!cmd.IsCorrectNumberOfParameters(1))
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Incorrect number of parameters for command %s", cmd.GetName());
		return;
	}
	Rgba color;
	std::string encounterName;

	encounterName = cmd.GetNextString();
	if(encounterName == "")
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Could not read encounter name (spaces are allowed if in \"\")");
		return;
	}

	EncounterDefinition* def = FindEncounterByName(encounterName);

	if(def == nullptr)
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Could not read encounter name (spaces are allowed if in \"\")");
		return;
	}

	CreateAndSetNewEncounterByDefinition(def);

	if(g_currentState != PLAYING_STATE)
	{
		g_currentState = PLAYING_STATE;
		g_transitionState = NONE_STATE;
	}

	DevConsolePrintf(color, "Successfully loaded encounter");//cmd.m_commandInfo->m_successMessage.c_str);
}

void CreateAndSetNewEncounterByDefinition(EncounterDefinition* encounterDef)
{
	//new off an encounter every time you go back and forth from the main menu
	delete(g_theEncounter);
	g_theEncounter = nullptr;

	g_theEncounter = new Encounter();
	g_theEncounter->m_definition = encounterDef;
	g_theEncounter->Initialize();
	g_theEncounter->GenerateNewMap();
}

EncounterDefinition* FindEncounterByName(const std::string& encounterName)
{
	std::map<std::string, EncounterDefinition*>::iterator encounterDefIterator = EncounterDefinition::s_encounterDefinitions.find(encounterName);
	if (encounterDefIterator != EncounterDefinition::s_encounterDefinitions.end())
	{
		return encounterDefIterator->second;		
	}	

	return nullptr;
}