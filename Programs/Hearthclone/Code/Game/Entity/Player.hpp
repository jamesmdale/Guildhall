#pragma once
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\Entity\Hero.hpp"
#include <vector>

enum ePlayerType
{
	SELF_PLAYER_TYPE,
	ENEMY_PLAYER_TYPE,
	NUM_PLAYER_TYPES
};

class PlayingState;
class Player
{
public:
	Player();
	~Player();

	void LoadDeckFromDefinitionName(const std::string& deckName);
	void UpdateDeckCount();
	void ShuffleDeck();

	//card position update methods
	void UpdateHandLockPositions();
	void UpdateBoardLockPositions();

public:
	std::vector<Card*> m_deck;
	std::vector<Card*> m_graveyard;
	std::vector<Card*> m_hand;
	std::vector<Minion*> m_minions;
	int m_manaCount = 0;
	Hero* m_hero = nullptr;

	PlayingState* m_gameState = nullptr;

	ePlayerType m_playerId = NUM_PLAYER_TYPES;
	int m_deckCount;	
	std::string m_displayName;
};

