#include "Game\Entity\Player.hpp"
#include "Game\Board.hpp"


Player::Player()
{
}


Player::~Player()
{
	m_class = nullptr;
	m_board = nullptr;

	
	for (int cardIndex = 0; cardIndex < (int)m_deck.size(); ++cardIndex)
	{
		m_deck[cardIndex] = nullptr;
	}
	m_deck.clear();

	for (int cardIndex = 0; cardIndex < (int)m_hand.size(); ++cardIndex)
	{
		m_hand[cardIndex] = nullptr;
	}
	m_hand.clear();

	for (int cardIndex = 0; cardIndex < (int)m_graveyard.size(); ++cardIndex)
	{
		m_graveyard[cardIndex] = nullptr;
	}
	m_graveyard.clear();

	for (int minionIndex = 0; minionIndex < (int)m_minions.size(); ++minionIndex)
	{
		m_minions[minionIndex] = nullptr;
	}
	m_minions.clear();
}
