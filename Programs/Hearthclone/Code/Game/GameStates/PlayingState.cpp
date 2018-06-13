#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Game\Entity\Card.hpp"

Card* card = nullptr;

enum ePlayState
{
	START_OF_GAME_PLAY_STATE,
	START_OF_TURN_PLAY_STATE,
	MAIN_PLAY_STATE,
	END_OF_TURN_PLAY_STATE,
	END_OF_GAME_PLAY_STATE,
	NUM_PLAY_STATES
};

PlayingState::~PlayingState()
{
	delete(m_gameBoard);
	m_gameBoard = nullptr;

	//delete scene last
	delete(m_renderScene2D);
	m_renderScene2D = nullptr;	

	m_player;
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	m_gameBoard = new Board("board");
	m_gameBoard->m_renderScene = m_renderScene2D;
	m_gameBoard->Initialize();

	//test a card
	card = new Card(CardDefinition::GetDefinitionByName("Chillwind Yeti"));
	card->m_renderScene = m_renderScene2D;
	card->Initialize();
	Vector2 clientCenter = Window::GetInstance()->GetCenterOfClientWindow();
	card->m_transform2D->SetLocalPosition(clientCenter);

	m_player = new Player();
	m_player->m_playerId = 0;
	m_player->m_gameState = this;
	m_player->LoadDeckFromDefinitionName("All Yetis");

	m_enemyPlayer = new Player();
	m_enemyPlayer->m_playerId = 1;
	m_enemyPlayer->m_gameState = this;
	m_enemyPlayer->LoadDeckFromDefinitionName("All Yetis");	

	theRenderer = nullptr;	
}

void PlayingState::Update(float deltaSeconds)
{ 
	//m_gameBoard->Update(deltaSeconds);
	card->Update(deltaSeconds);
	
}

void PlayingState::PreRender()
{
	//m_gameBoard->PreRender();
	card->PreRender();
}

void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Game::GetInstance()->m_forwardRenderingPath2D->Render(m_renderScene2D);

	theRenderer = nullptr;
}

void PlayingState::PostRender()
{
	//post render steps here.
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	std::string mouseText = "NONE";

	if (theInput->IsKeyPressed(theInput->MOUSE_LEFT_CLICK))
	{
		if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
		{
			mouseText = "Just pressed left click";
		}
		else
		{
			mouseText = "Is currently pressed left click";
		}
		
	}
	if (theInput->IsKeyPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
		{
			mouseText = "Just pressed right click";
		}
		else
		{
			mouseText = "Is currently pressed right click"; 
		}		
	}	
	
	if (theInput->WasKeyJustReleased(theInput->MOUSE_LEFT_CLICK))
	{
		mouseText = "Just released left click";
	}
	if (theInput->WasKeyJustReleased(theInput->MOUSE_RIGHT_CLICK))
	{
		mouseText = "Just released right click";
	}

	if (theInput->GetMouseDoubleClickLeft())
	{
		mouseText = "double clicked left";
	}
	if (theInput->GetMouseDoubleClickRight())
	{
		mouseText = "double clicked right";
	}

	DebugRender::GetInstance()->CreateDebugText2D(Vector2(Window::GetInstance()->m_clientWidth - 300, Window::GetInstance()->m_clientHeight - 20), 20.f, 1.f, mouseText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);

	theInput = nullptr;

	return deltaSeconds; //new deltaSeconds
}

void UpdateStartGame()
{
	
}

void UpdateStartTurn()
{
	//do things that happen at turn start. triggers can happen here

}

void UpdateMain()
{
	//do things that happen at turn start. triggers can happen here

}

void UpdateEndTurn()
{

}

