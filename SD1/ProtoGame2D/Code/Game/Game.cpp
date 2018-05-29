#include "Game.hpp"
#include <stdlib.h>
#include "Engine\Math\MathUtils.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"


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

}

void Game::Render()
{
	g_theRenderer->DrawLineWithColor(g_bottomLeftOrtho, g_topRightOrtho * .5, Rgba(1.f, 1.f, 1.f, 1.f));
}

float Game::UpdateFlagsFromInput(float timeDelta)
{
	return 0.f;
}

