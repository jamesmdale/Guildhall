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
}

void Game::Initialize()
{
	//initialize game camera
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	g_gameClock = new Clock(GetMasterClock());
}


