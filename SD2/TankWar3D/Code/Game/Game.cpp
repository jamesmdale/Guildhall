#include "Game.hpp"
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
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\GameObject.hpp"
#include <vector>
#include <string>
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"

Game* g_theGame = nullptr;
Clock* g_gameClock = nullptr;

GameState g_currentState;
GameState g_transitionState;
float g_secondsInState;
float g_secondsTransistioning;
bool g_isFinishedTransistioning;

bool m_isPaused = false;


Game::Game()
{
	m_renderScene = new RenderScene();
	m_forwardRenderingPath = new ForwardRenderingPath();
}

Game::~Game()
{
	delete(m_forwardRenderingPath);
	m_forwardRenderingPath = nullptr;

	delete(m_renderScene);
	m_renderScene = nullptr;	

	delete(m_gameCamera);
	m_gameCamera = nullptr;

	delete(m_uiCamera);
	m_uiCamera = nullptr;

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
	m_gameCamera->SetPerspective(60.f, CLIENT_ASPECT, 0.1f, 10000.f);
	m_gameCamera->Translate(Vector3(0.f, 5.f, -20.f));

	m_renderScene->AddCamera(m_gameCamera); 

	//cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

void Game::Update(float timeDelta)
{
	if (g_transitionState != NONE_STATE)
	{
		g_currentState = g_transitionState;
		g_transitionState = NONE_STATE;
		g_secondsInState = 0.f;
	}

	switch (g_currentState)
	{
	case MAIN_MENU_STATE:
		UpdateMainMenu(timeDelta);
		break;
	case LOADING_STATE:
		UpdateLoading(timeDelta);
		break;
	case PLAYING_STATE:
		UpdateGame(timeDelta);
		break;
	}

	g_secondsInState += timeDelta;
}



void Game::PreRender()
{
	//add any prerender tasks here
}

void Game::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_gameCamera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	//render from forward rendering path
	m_forwardRenderingPath->Render(m_renderScene);

	theRenderer = nullptr;
}

void Game::PostRender()
{
	//add any postrender tasks here
}


float Game::UpdateInput(float timeDelta)
{
	//update any input here

	return timeDelta;
}

//State management
void Game::TransitionGameStates(GameState toState)
{
	g_transitionState = toState;
}


void Game::Render()
{
	switch (g_currentState)
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




