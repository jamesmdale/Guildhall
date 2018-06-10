#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Engine\Core\Terrain.hpp"
#include "Game\Board.hpp"

class Tank;
class PlayingState : public GameState
{
public:
	PlayingState(Camera* camera) : GameState(camera)
	{
		m_type = PLAYING_GAME_STATE;
	}

	virtual ~PlayingState() override;
	
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

public:
	Board* m_gameBoard = nullptr;

	//Player* playerOne = nullptr;
	//Player2* playerTwo = nullptr; //player contains hand, graveyard, their side of the battlefield, and their class
	//Battlefield* battlefield = nullptr;
	//MatchLog

};

