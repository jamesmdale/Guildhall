#include <stdlib.h>
#include "Game\Game.hpp"
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
#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\GameCommon.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\MainMenuState.hpp"
#include "Game\GameStates\LoadingState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetMessage.hpp"

#include <vector>
#include <string>

//game instance
static Game* g_theGame = nullptr;

bool m_isPaused = false;

//  =========================================================================================
Game::Game()
{
	m_forwardRenderingPath2D = new ForwardRenderingPath2D();
}

//  =========================================================================================
Game::~Game()
{
	// delete render members
	delete(m_forwardRenderingPath2D);
	m_forwardRenderingPath2D = nullptr;

	// delete camera members
	delete(m_gameCamera);
	m_gameCamera = nullptr;

	delete(m_gameCamera);
	m_gameCamera = nullptr;

	//cleanup global members

	//add any other data to cleanup
}

//  =========================================================================================
Game* Game::GetInstance()
{
	return g_theGame;
}

//  =========================================================================================
Game* Game::CreateInstance()
{
	if (g_theGame == nullptr)
	{
		g_theGame = new Game();
	}

	return g_theGame;
}

//  =========================================================================================
void Game::Initialize()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	m_gameClock = new Clock(GetMasterClock());

	//command
	CommandRegister("help", CommandRegistration(Help, ": Use to show all supported commands", "All commands displayed!"));


	// add cameras
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_gameCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_gameCamera->SetView(Matrix44::IDENTITY);

	// add menu states
	TODO("Add other menu states");
	GameState::AddGameState(new MainMenuState(m_gameCamera));
	GameState::AddGameState(new LoadingState(m_gameCamera));
	GameState::AddGameState(new PlayingState(m_gameCamera));

	// set to initial menu
	GameState::TransitionGameStatesImmediate(GameState::GetGameStateFromGlobalListByType(MAIN_MENU_GAME_STATE));
	GameState::UpdateGlobalGameState(0.f);

	//register actions
	RegisterAllActions();

	// load definitions
	CardDefinition::Initialize("Data/Definitions/Cards/cards.xml");
	HeroDefinition::Initialize("Data/Definitions/Heroes/heroes.xml");
	DeckDefinition::Initialize("Data/Definitions/Decks/decks.xml");

	// cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

//  =========================================================================================
void Game::Update()
{
	float deltaSeconds = m_gameClock->GetDeltaSeconds();

	// update global menu data (handles transitions and timers) =============================================================================
	GameState::UpdateGlobalGameState(deltaSeconds);

	GameState::GetCurrentGameState()->Update(deltaSeconds);
}

//  =========================================================================================
void Game::PreRender()
{
	GameState::GetCurrentGameState()->PreRender();
}

//  =========================================================================================
void Game::Render()
{
	GameState::GetCurrentGameState()->Render();
}

//  =========================================================================================
void Game::PostRender()
{
	GameState::GetCurrentGameState()->PostRender();
}

//  =============================================================================
void Game::RegisterGameMessages()
{
	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->RegisterMessageDefinition(UNRELAIBLE_TEST_GAME_NET_MESSAGE_TYPE, "unreliable_test", OnUnreliableTest);
	theNetSession->RegisterMessageDefinition("test", OnTest);

	theNetSession = nullptr;
}

//  =========================================================================================
float Game::UpdateInput(float deltaSeconds)
{
	deltaSeconds = GameState::GetCurrentGameState()->UpdateFromInput(deltaSeconds);

	return deltaSeconds;
}

//  =============================================================================
void UnreliableTest(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetConnectionById(connectionIndex);
	if(connection == nullptr)
		DevConsolePrintf("Connection index invalid");

	int numSends = cmd.GetNextInt();
	if (numSends <= 0)
	{
		DevConsolePrintf("Invalid number of sends");
	}


}

//  =============================================================================
//	Net Callbacks
//  =============================================================================
bool OnUnreliableTest(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =============================================================================
bool OnTest(NetMessage & message, NetConnection * fromConnection)
{
	//out of order should still work
	return false;
}



