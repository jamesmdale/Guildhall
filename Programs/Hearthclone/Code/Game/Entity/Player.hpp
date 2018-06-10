#pragma once
#include "Game\Entity\Card.hpp"
#include "Game\Entity\HeroClass.hpp"
#include "Game\Entity\Minion.hpp"
#include <vector>

class Board;
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
	HeroClass* m_class = nullptr;

	Board* m_board = nullptr;

	int m_playerId;
	int m_deckCount;
	int m_health;
	int m_armor;
	int m_mana;
	int m_attackPower;
	
	std::string name;
};

