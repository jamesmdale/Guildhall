#include "Game\TurnStates\TurnStateManager.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include <string>
#include <map>
#include <vector>
#include "Game\Entity\Player.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\StringUtils.hpp"

bool isFinishedTransitioningIn = true;
bool isFinishedTransitioningOut = true;


TurnStateManager::TurnStateManager()
{
	m_currentState = NUM_PLAY_STATES;
	TransitionToState(START_OF_GAME_PLAY_STATE);
}

TurnStateManager::~TurnStateManager()
{
	m_playingState = nullptr;
}

void TurnStateManager::Update(float deltaSeconds)
{
	if (m_transitionState != NUM_PLAY_STATES)
	{
		Transition();
	}
	//else
	//{
	//	switch (m_currentState)
	//	{
	//	case START_OF_GAME_PLAY_STATE:
	//		UpdateStartOfGame(deltaSeconds);
	//		break;
	//	case START_OF_TURN_PLAY_STATE:
	//		UpdateStartOfTurn(deltaSeconds);
	//		break;
	//	case MAIN_PLAY_STATE:
	//		UpdateMain(deltaSeconds);
	//		break;
	//	case END_OF_TURN_PLAY_STATE:
	//		UpdateEndOfTurn(deltaSeconds);
	//		break;
	//	case END_OF_GAME_PLAY_STATE:
	//		UpdateEndOfGame(deltaSeconds);
	//		break;
	//	}
	//}	
}

float TurnStateManager::UpdateFromInput(float deltaSeconds)
{
	if (m_transitionState != NUM_PLAY_STATES)
	{
		switch (m_currentState)
		{
		case START_OF_GAME_PLAY_STATE:
			deltaSeconds = UpdateInputStartOfGame(deltaSeconds);
			break;
		case START_OF_TURN_PLAY_STATE:
			deltaSeconds = UpdateInputStartOfTurn(deltaSeconds);
			break;
		case MAIN_PLAY_STATE:
			deltaSeconds = UpdateInputMain(deltaSeconds);
			break;
		case END_OF_TURN_PLAY_STATE:
			deltaSeconds = UpdateInputEndOfTurn(deltaSeconds);
			break;
		case END_OF_GAME_PLAY_STATE:
			deltaSeconds = UpdateInputEndOfGame(deltaSeconds);
			break;
		}
	}

	return deltaSeconds;
}


void TurnStateManager::TransitionToState(ePlayState toState)
{
	m_transitionState = toState;
	isFinishedTransitioningIn = false;
	isFinishedTransitioningOut = false;
}

void TurnStateManager::Transition()
{
	//if we are still transitioning out run the current state's transition out

	if (isFinishedTransitioningOut == false)
	{
		switch (m_currentState)
		{
		case START_OF_GAME_PLAY_STATE:
			TransitionOutStartOfGame();
			break;
		case START_OF_TURN_PLAY_STATE:
			TransitionOutStartOfTurn();
			break;
		case MAIN_PLAY_STATE:
			TransitionOutMain();
			break;
		case END_OF_TURN_PLAY_STATE:
			TransitionOutEndOfTurn();
			break;
		case END_OF_GAME_PLAY_STATE:
			TransitionOutEndOfGame();
			break;
		case NUM_PLAY_STATES:
			isFinishedTransitioningOut = true;
			break;
		}
	}
	else if (isFinishedTransitioningIn == false)
	{
		switch (m_transitionState)
		{
		case START_OF_GAME_PLAY_STATE:
			TransitionInStartOfGame();
			break;
		case START_OF_TURN_PLAY_STATE:
			TransitionInStartOfTurn();
			break;
		case MAIN_PLAY_STATE:
			TransitionInMain();
			break;
		case END_OF_TURN_PLAY_STATE:
			TransitionInEndOfTurn();
			break;
		case END_OF_GAME_PLAY_STATE:
			TransitionInEndOfGame();
			break;
		case NUM_PLAY_STATES:
			isFinishedTransitioningIn = true;
			break;
		}
	}
	//we are finished transitioning out of the old state and end to the new.
	else
	{
		m_currentState = m_transitionState;
		m_transitionState = NUM_PLAY_STATES;
	}
	
}

// update methods =========================================================================================
//void TurnStateManager::UpdateStartOfGame(float deltaSeconds)
//{
//
//}
//
//void TurnStateManager::UpdateStartOfTurn(float deltaSeconds)
//{
//
//}
//
//
//void TurnStateManager::UpdateMain(float deltaSeconds)
//{
//
//}
//
//void TurnStateManager::UpdateEndOfTurn(float deltaSeconds)
//{
//
//}
//
//void TurnStateManager::UpdateEndOfGame(float deltaSeconds)
//{
//
//}

void TurnStateManager::TransitionInStartOfGame()
{
	//each player draws 3 cards
	std::map<std::string, std::string> parameters = {{"target", "player"}, {"amount", "3"}};
	AddActionToRefereeQueue("draw", parameters);

	//each player draws 3 cards
	parameters = {{"target", "enemy"}, {"amount", "3"}};
	AddActionToRefereeQueue("draw", parameters);

	TODO("roll random for who starts first");
	m_playingState->m_activePlayer = m_playingState->m_player;

	isFinishedTransitioningIn = true;
}

void TurnStateManager::TransitionInStartOfTurn()
{
	//swap active player
	if (m_playingState->m_activePlayer->m_playerId == SELF_PLAYER_TYPE)
		m_playingState->m_activePlayer = m_playingState->m_enemyPlayer;
	else
		m_playingState->m_activePlayer = m_playingState->m_player;

	//add one mana crystal if below max
	if(m_playingState->m_player->m_manaCount < 10)
		m_playingState->m_player->m_manaCount++;

	//draw a card
	std::map<std::string, std::string> parameters = {{"target", "player"}, {"amount", "1"}};
	AddActionToRefereeQueue("draw", parameters);

	isFinishedTransitioningIn = true;
}

void TurnStateManager::TransitionInMain()
{
	isFinishedTransitioningIn = true;
}

void TurnStateManager::TransitionInEndOfTurn()
{
	isFinishedTransitioningIn = true;
}

void TurnStateManager::TransitionInEndOfGame()
{
	isFinishedTransitioningIn = true;
}

// transition out =============================================================================
#pragma region transition_out

void TurnStateManager::TransitionOutStartOfGame()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutStartOfTurn()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutMain()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutEndOfTurn()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutEndOfGame()
{
	isFinishedTransitioningOut = true;
}
#pragma endregion

// update input  =============================================================================
#pragma region Input

float TurnStateManager::UpdateInputStartOfGame(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputStartOfTurn(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputMain(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	std::string mouseText = "NONE";

	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	std::vector<Widget*> interactableWidgets = *m_playingState->GetInteractableWidgets();

	Widget* currentSelectedWidget = m_playingState->m_currentSelectedWidget;

	//basically does nothing
	if (theInput->WasKeyJustReleased(theInput->MOUSE_LEFT_CLICK))
	{
		if (currentSelectedWidget != nullptr)
		{
			currentSelectedWidget->OnLeftReleased();
		}	
	}
	//basically does nothing
	if (theInput->WasKeyJustReleased(theInput->MOUSE_RIGHT_CLICK))
	{
		if (currentSelectedWidget != nullptr)
		{
			currentSelectedWidget->OnRightReleased();
		}
	}	

	//left click input is only available to the current player
	if (theInput->IsKeyPressed(theInput->MOUSE_LEFT_CLICK) && m_playingState->m_activePlayer->m_playerId == SELF_PLAYER_TYPE)
	{
		if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
		{
			if (currentSelectedWidget == nullptr)
			{
				currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
			}
			//if the previously selected widget isn't holding input priority, we can select a new widget
			else
			{
				if (!currentSelectedWidget->m_isInputPriority)
				{
					currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
				}
			}


			if(currentSelectedWidget != nullptr)
				currentSelectedWidget->OnLeftClicked();
		}
	}
	if (theInput->IsKeyPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
		{			
			if (currentSelectedWidget == nullptr)
			{
				currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
			}
			else
			{
				//if the previously selected widget isn't holding input priority, we can select a new widget
				if (!currentSelectedWidget->m_isInputPriority)
				{
					currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
				}
			}		

			if(currentSelectedWidget != nullptr)
				currentSelectedWidget->OnRightClicked();
		}
	}
	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_1))
	{
		std::map<std::string, std::string> parameters = {{"target", "player"}, {"amount", "2"}};
		AddActionToRefereeQueue("draw", parameters);
	}

	DebugRender::GetInstance()->CreateDebugText2D(Vector2(Window::GetInstance()->m_clientWidth - 300, Window::GetInstance()->m_clientHeight - 20), 20.f, 1.f, Stringf("%f, %f", mouseCoordinates.x, mouseCoordinates.y).c_str(), Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);

	// cleanup =========================================================================================
	for (int widgetIndex = 0; widgetIndex < (int)interactableWidgets.size(); ++widgetIndex)
	{
		interactableWidgets[widgetIndex] = nullptr;
	}
	interactableWidgets.clear();

	theInput = nullptr;

	// return =============================================================================
	return deltaSeconds;
}

float TurnStateManager::UpdateInputEndOfTurn(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputEndOfGame(float deltaSeconds)
{
	return deltaSeconds;
}

#pragma endregion








