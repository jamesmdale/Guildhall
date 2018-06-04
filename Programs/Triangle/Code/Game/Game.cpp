#include "Game.hpp"
#include <stdlib.h>
#include "Engine/Math/MathUtils.hpp"
#include "engine/renderer/renderer.hpp"
#include "game/GameCommon.hpp"
#include "engine/Math/Matrix44.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"


Game* g_Game = nullptr;

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::Initialize()
{
	
}

void Game::Update(float timeDelta)
{		
	UpdateFlagsFromInput(timeDelta);
}

void Game::Render()
{
	TODO("Need to get bounds from hwnd instead of manually");

	AABB2 bounds = AABB2(0.f, 0.f, 200.f, 200.f);
	//g_theRenderer->DrawAABB(bounds, Rgba(0.f, 0.f, 1.f, 1.f));
	AABB2 bounds2 = AABB2(50.f, 50.f, 250.f, 250.f);
	AABB2 bounds3 = AABB2(100.f, 100.f, 100.f, 300.f);

	Texture* testImage = g_theRenderer->CreateOrGetTexture("Data/Images/Killer1.png");
	g_theRenderer->DrawTexturedAABB(bounds, *testImage, Vector2(0.f, 1.f), Vector2(1.f, 0.f), Rgba(1.f, 1.f, 0.f, 1.f));

	Texture* testImage2 = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	g_theRenderer->DrawTexturedAABB(bounds2, *testImage2, Vector2(0.f, 1.f), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, .3f));
}

float Game::UpdateFlagsFromInput(float timeDelta)
{
	UNUSED(timeDelta);
	if(!DevConsole::GetInstance()->IsOpen())
	{
		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_F9)))
		{
			g_theRenderer->ReloadShaders();
		}

		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_0)))
		{
			g_theRenderer->LoadErrorShader();
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
	return 0.f;
}

