#include <stdlib.h>
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Menu\MenuState.hpp"
#include "Game\Menu\MainMenuState.hpp"
#include "Game\Menu\ReadyState.hpp"
#include "Game\Menu\LoadingState.hpp"
#include "Game\Menu\PlayingState.hpp"
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
#include "Engine\Core\GameObject.hpp"
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
	m_forwardRenderingPath = new ForwardRenderingPath();
}

Game::~Game()
{
	//delete render members
	delete(m_forwardRenderingPath);
	m_forwardRenderingPath = nullptr;

	//delete camera members
	delete(m_gameCamera);
	m_gameCamera = nullptr;

	delete(m_uiCamera);
	m_uiCamera = nullptr;

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
	m_gameCamera->SetDepthStencilTarget(theRenderer->GetDefaultDepthStencilTarget());
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_gameCamera->SetPerspective(60.f, CLIENT_ASPECT, 0.1f, 10000.f);
	m_gameCamera->Translate(Vector3::ZERO);

	m_gameCamera->m_skybox = new Skybox("Data/Images/galaxy2.png");

	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_uiCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_uiCamera->SetView(Matrix44::IDENTITY);

	//add menu states
	TODO("Add other menu states");
	MenuState::AddMenuState(new LoadingState(m_uiCamera));
	MenuState::AddMenuState(new MainMenuState(m_uiCamera));
	MenuState::AddMenuState(new ReadyState(m_uiCamera));
	MenuState::AddMenuState(new PlayingState(m_gameCamera));
	//loadingMenuState
	//readyUp
	//play/level

	//set to initial menu
	MenuState::TransitionMenuStatesImmediate(MenuState::GetMenuStateFromGlobalListByType(LOADING_MENU_STATE));
	MenuState::UpdateGlobalMenuState(0);

	//cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

void Game::Update()
{
	float deltaSeconds = m_gameClock->GetDeltaSeconds();

	MenuState::GetCurrentMenuState()->Update(deltaSeconds);
}

void Game::PreRender()
{
	MenuState::GetCurrentMenuState()->PreRender();
}

void Game::Render()
{
	MenuState::GetCurrentMenuState()->Render();
}

void Game::PostRender()
{
	MenuState::GetCurrentMenuState()->PostRender();
}

float Game::UpdateInput(float deltaSeconds)
{
	deltaSeconds = MenuState::GetCurrentMenuState()->UpdateFromInput(deltaSeconds);

	return deltaSeconds;
}


