#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include <map>
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\TurnStates\TurnStateManager.hpp"


PlayingState::~PlayingState()
{
	delete(m_turnStateManager);
	m_turnStateManager = nullptr;

	delete(m_gameBoard);
	m_gameBoard = nullptr;

	delete(m_player);
	m_player = nullptr;

	delete(m_enemyPlayer);
	m_enemyPlayer = nullptr;

	delete(m_currentSelectedWidget);
	m_enemyPlayer = nullptr;

	//delete scene last
	delete(m_renderScene2D);
	m_renderScene2D = nullptr;		
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	m_turnStateManager = new TurnStateManager();
	m_turnStateManager->m_playingState = this;

	m_gameBoard = new Board("board");
	m_gameBoard->m_renderScene = m_renderScene2D;
	m_gameBoard->Initialize();

	//add players
	m_player = new Player();
	m_player->m_playerId = SELF_PLAYER_TYPE;
	m_player->m_gameState = this;
	m_player->LoadDeckFromDefinitionName("All Yetis");

	//load their decks
	m_enemyPlayer = new Player();
	m_enemyPlayer->m_playerId = ENEMY_PLAYER_TYPE;
	m_enemyPlayer->m_gameState = this;
	m_enemyPlayer->LoadDeckFromDefinitionName("All Yetis");	

	//start game time
	m_gameTime = new Stopwatch();
	m_gameTime->SetClock(GetMasterClock());

	m_activePlayer = m_player;	

	//cleanup
	theRenderer = nullptr;	
}

void PlayingState::Update(float deltaSeconds)
{ 
	//process queues
	ProcessEffectQueue();
	ProcessRefereeQueue();

	//if we are processing actions and effects, don't allow turn state to update.
	if (GetEffectQueueCount() == 0 && GetRefereeQueueCount() == 0)
	{
		//update turn state manager
		m_turnStateManager->Update(deltaSeconds);
	}	
		
	//update enemy
	m_enemyPlayer->Update(deltaSeconds);

	//update self
	m_player->Update(deltaSeconds);

}

void PlayingState::PreRender()
{
	//run prerender for enemy player
	m_enemyPlayer->PreRender();

	//run prerender for player
	m_player->PreRender();	
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
	m_turnStateManager->UpdateFromInput(deltaSeconds);	

	// return 
	return deltaSeconds; //new deltaSeconds
}

std::vector<Widget*>* PlayingState::GetInteractableWidgets()
{
	std::vector<Widget*>* interactableWidgets = new std::vector<Widget*>();

	// player =========================================================================================
	//add widgets from player's hand
	for (int widgetIndex = 0; widgetIndex < (int)m_player->m_hand.size(); ++widgetIndex)
	{
		interactableWidgets->push_back(m_player->m_hand[widgetIndex]);
	}

	//add widgets from player's minions
	for (int widgetIndex = 0; widgetIndex < (int)m_player->m_minions.size(); ++widgetIndex)
	{
		interactableWidgets->push_back(m_player->m_minions[widgetIndex]);
	}
	
	TODO("Add hero input update");
	//add hero widget
	//interactableWidgets.push_back(m_player->m_hero);


	// enemy player =========================================================================================
	//add widgets from enemy player's hand
	for (int widgetIndex = 0; widgetIndex < (int)m_enemyPlayer->m_hand.size(); ++widgetIndex)
	{
		interactableWidgets->push_back(m_enemyPlayer->m_hand[widgetIndex]);
	}

	//add widgets from enemy player's battlefield
	for (int widgetIndex = 0; widgetIndex < (int)m_enemyPlayer->m_minions.size(); ++widgetIndex)
	{
		interactableWidgets->push_back(m_enemyPlayer->m_minions[widgetIndex]);
	}

	TODO("Add hero input update");
	//add hero widget
	//interactableWidgets.push_back(m_enemyPlayer->m_hero);

	TODO("Add gameboard update");
	// game board =========================================================================================

	//return
	return interactableWidgets;
}

Widget* PlayingState::GetSelectedWidget(const std::vector<Widget*>& interactableWidgets)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();

	//create a vector of widgets that are under the mouse position.  We will sort according to layer to find selected widget
	Widget* selectedWidget = nullptr;

	//add each widget at the mouse cursor to 
	for (int widgetIndex = 0; widgetIndex < (int)interactableWidgets.size(); ++widgetIndex)
	{
		Widget* widget = interactableWidgets[widgetIndex];
		Vector2 position = widget->m_transform2D->GetWorldPosition();
		AABB2 widgetBounds = AABB2(position, widget->m_dimensionsInPixels.x, widget->m_dimensionsInPixels.y);

		if (widgetBounds.IsPointInside(mousePosition) == true)
		{
			if (selectedWidget == nullptr )
			{
				selectedWidget = widget;
			}
			else if (widget->GetSortLayer() > selectedWidget->GetSortLayer())
			{
				selectedWidget = widget;
			}
		}

		widget = nullptr;
	}

	// return =========================================================================================
	return selectedWidget;
}

