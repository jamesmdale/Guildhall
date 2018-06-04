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


Game* g_theGame = nullptr;
Clock* g_gameClock = nullptr;

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::Initialize()
{

	//initialize game camera
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	m_gameCamera = new Camera();
	m_gameCamera->SetDepthStencilTarget(theRenderer->GetDefaultDepthStencilTarget());
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());	
	m_gameCamera->SetProjectionOrtho(10.f, CLIENT_ASPECT, -200.f, 200.f);

	//initialize UI camera
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_uiCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_uiCamera->SetView(Matrix44::IDENTITY);

	theRenderer = nullptr;
	theWindow = nullptr;

	g_gameClock = new Clock(GetMasterClock());
}

void Game::Update(float timeDelta)
{		
	UpdateFlagsFromInput(timeDelta);
}

void Game::Render()
{
	TODO("Need to get bounds from hwnd instead of manually");
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_uiCamera);

	AABB2 bounds = AABB2(0.f, 0.f, 200.f, 200.f);
	AABB2 bounds2 = AABB2(50.f, 50.f, 250.f, 250.f);
	AABB2 bounds3 = AABB2(100.f, 100.f, 100.f, 300.f);

	Texture* testImage = theRenderer->CreateOrGetTexture("Data/Images/Killer1.png");
	theRenderer->DrawTexturedAABB(bounds, *testImage, Vector2(0.f, 1.f), Vector2(1.f, 0.f), Rgba(1.f, 1.f, 0.f, 1.f));

	Texture* testImage2 = theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	theRenderer->DrawTexturedAABB(bounds2, *testImage2, Vector2(0.f, 1.f), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, .3f));

	theRenderer = nullptr;
}

float Game::UpdateFlagsFromInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();

	UNUSED(timeDelta);
	if(!DevConsole::GetInstance()->IsOpen())
	{
		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_F9)))
		{
			theRenderer->ReloadShaders();
		}

		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_0)))
		{
			theRenderer->LoadErrorShader();
		}

		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_Q)))
		{
			CommandRun("quit");
		}
		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ADD)))
		{
			CommandRun("add 5 2");
		}		
	}	

	theRenderer = nullptr;
	return 0.f;
}

