#pragma once
#include "Game\GameStates\GameState.hpp"

enum ePlayState
{
	START_OF_GAME_PLAY_STATE,
	START_OF_TURN_PLAY_STATE,
	MAIN_PLAY_STATE,
	END_OF_TURN_PLAY_STATE,
	END_OF_GAME_PLAY_STATE,
	NUM_PLAY_STATES
};

class TurnState
{
public:
	TurnState();
	~TurnState();

	void Update(float deltaSeconds);
	void TransitionIn();
	void TransitionOut();
	void TransitionTurnStates();

private:
	//update methods
	void UpdateStartOfGame(float deltaSeconds);
	void UpdateStartOfTurn(float deltaSeconds);
	void UpdateMain(float deltaSeconds);
	void UpdateEndOfTurn(float deltaSeconds);
	void UpdateEndOfGame(float deltaSeconds);

public:
	PlayingState* m_playingState = nullptr;
	ePlayState m_state = NUM_PLAY_STATES;
};