#pragma once
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\Entity\Hero.hpp"
#include <vector>

class PlayingState;
class Player
{
public:
	Player();
	~Player();

public:
	std::vector<Card*> m_deck;
	std::vector<Card*> m_graveyard;
	std::vector<Card*> m_hand;
	std::vector<Minion*> m_minions;
	Hero* m_hero = nullptr;

	PlayingState* m_gameState = nullptr;

	int m_playerId;
	int m_deckCount;
	
	std::string name;
};

