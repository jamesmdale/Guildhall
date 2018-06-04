#include "Game\Game.hpp"
#include <stdlib.h>
#include "Engine\Math\MathUtils.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"
#include "engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\GameObject.hpp"
#include <vector>
#include <string>
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Game\Menu\MenuState.hpp"

Game* g_theGame = nullptr;
Clock* g_gameClock = nullptr;

bool m_isPaused = false;

Game::Game()
{
	m_forwardRenderingPath = new ForwardRenderingPath();
}

Game::~Game()
{
	delete(m_forwardRenderingPath);
	m_forwardRenderingPath = nullptr;

	delete(m_gameCamera);
	m_gameCamera = nullptr;

	//add any other data to cleanup
}

void Game::Initialize()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	g_gameClock = new Clock(GetMasterClock());

	//Add camera
	m_gameCamera = new Camera();
	m_gameCamera->SetDepthStencilTarget(theRenderer->GetDefaultDepthStencilTarget());
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_gameCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_gameCamera->SetView(Matrix44::IDENTITY);

	TODO("Create MenuStates");

	//cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

void Game::Update(float timeDelta)
{
	MenuState::s_currentState->Update(timeDelta);
}

void Game::PreRender()
{
	MenuState::s_currentState->PreRender();
}

void Game::Render()
{
	MenuState::s_currentState->Render();
}

void Game::PostRender()
{
	MenuState::s_currentState->PostRender();
}


float Game::UpdateInput(float timeDelta)
{
	float adjustedTimeDelta = MenuState::s_currentState->UpdateFromInput(timeDelta);

	return adjustedTimeDelta;
}
