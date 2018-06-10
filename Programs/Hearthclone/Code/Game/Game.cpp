#include <stdlib.h>
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\MainMenuState.hpp"
#include "Game\GameStates\LoadingState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\Widget.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <vector>
#include <string>

//game instance
static Game* g_theGame = nullptr;

bool m_isPaused = false;

Game::Game()
{
	m_forwardRenderingPath2D = new ForwardRenderingPath2D();
}

Game::~Game()
{
	//delete render members
	delete(m_forwardRenderingPath2D);
	m_forwardRenderingPath2D = nullptr;

	//delete camera members
	delete(m_gameCamera);
	m_gameCamera = nullptr;

	delete(m_gameCamera);
	m_gameCamera = nullptr;

	//cleanup global members

	//add any other data to cleanup
}

Game* Game::GetInstance()
{
	return g_theGame;
}

Game* Game::CreateInstance()
{
	if (g_theGame == nullptr)
	{
		g_theGame = new Game();
	}

	return g_theGame;
}

void Game::Initialize()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	m_gameClock = new Clock(GetMasterClock());

	//Add camera
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_gameCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_gameCamera->SetView(Matrix44::IDENTITY);

	//add menu states
	TODO("Add other menu states");
	GameState::AddGameState(new MainMenuState(m_gameCamera));
	GameState::AddGameState(new LoadingState(m_gameCamera));
	GameState::AddGameState(new PlayingState(m_gameCamera));

	//set to initial menu
	GameState::TransitionGameStatesImmediate(GameState::GetGameStateFromGlobalListByType(MAIN_MENU_GAME_STATE));
	GameState::UpdateGlobalGameState(0.f);

	//cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

void Game::Update()
{
	float deltaSeconds = m_gameClock->GetDeltaSeconds();

	//update global menu data (handles transitions and timers)
	GameState::UpdateGlobalGameState(deltaSeconds);

	GameState::GetCurrentGameState()->Update(deltaSeconds);
}

void Game::PreRender()
{
	GameState::GetCurrentGameState()->PreRender();
}

void Game::Render()
{
	GameState::GetCurrentGameState()->Render();
}

void Game::PostRender()
{
	GameState::GetCurrentGameState()->PostRender();
}

float Game::UpdateInput(float deltaSeconds)
{
	deltaSeconds = GameState::GetCurrentGameState()->UpdateFromInput(deltaSeconds);

	return deltaSeconds;
}


