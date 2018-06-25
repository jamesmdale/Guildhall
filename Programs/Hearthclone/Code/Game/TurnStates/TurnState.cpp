#include "Game\TurnStates\TurnState.hpp"
#include "Game\GameStates\PlayingState.hpp"

bool isTransitioning = false;

TurnState::TurnState()
{
}


TurnState::~TurnState()
{
	m_playingState = nullptr;
}

void TurnState::Update(float deltaSeconds)
{
	switch (m_state)
	{
	case START_OF_GAME_PLAY_STATE:
		UpdateStartOfGame(deltaSeconds);
		break;
	case START_OF_TURN_PLAY_STATE:
		UpdateStartOfTurn(deltaSeconds);
		break;
	case MAIN_PLAY_STATE:
		UpdateMain(deltaSeconds);
		break;
	case END_OF_TURN_PLAY_STATE:
		UpdateEndOfTurn(deltaSeconds);
		break;
	case END_OF_GAME_PLAY_STATE:
		UpdateEndOfGame(deltaSeconds);
		break;
	}
}

void TurnState::TransitionIn()
{
}

void TurnState::TransitionOut()
{
}

void TurnState::TransitionTurnStates()
{
}

// update methods =========================================================================================
void TurnState::UpdateStartOfGame(float deltaSeconds)
{

}

void TurnState::UpdateStartOfTurn(float deltaSeconds)
{

}


void TurnState::UpdateMain(float deltaSeconds)
{

}

void TurnState::UpdateEndOfTurn(float deltaSeconds)
{

}

void TurnState::UpdateEndOfGame(float deltaSeconds)
{

}

