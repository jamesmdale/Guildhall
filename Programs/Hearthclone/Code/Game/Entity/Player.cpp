#include "Game\Entity\Player.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Definitions\CardDefinition.hpp"
#include "Game\Definitions\DeckDefinition.hpp"


Player::Player()
{
}


Player::~Player()
{
	m_hero = nullptr;
	m_gameState = nullptr;

	
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

void Player::LoadDeckFromDefinitionName(const std::string& deckName)
{
	DeckDefinition* deck = DeckDefinition::GetDefinitionByName(deckName);

	// clear contents of current deck =============================================================================
	if ((int)m_deck.size() > 0)
	{
		for (int cardIndex = 0; cardIndex < (int)m_deck.size(); ++cardIndex)
		{
			m_deck[cardIndex] = nullptr;
		}
		m_deck.clear();
		m_deck.shrink_to_fit();
	}
	
	// load deck =============================================================================
	for (int cardIndex = 0; cardIndex < (int)deck->m_cardNames.size(); ++cardIndex)
	{
		Card* cardToAdd = new Card(CardDefinition::GetDefinitionByName(deck->m_cardNames[cardIndex]));
		m_deck.push_back(cardToAdd);
		cardToAdd = nullptr;
	}

	deck = nullptr;
	UpdateDeckCount();
}

void Player::UpdateDeckCount()
{
	if (m_deckCount > 0)
	{
		m_deckCount = (int)m_deck.size();
	}
}


void Player::ShuffleDeck()
{
	//not totally random would need to revisit this, but will do for now.
	for (int shuffleCount = 0; shuffleCount < (int)m_deck.size(); ++shuffleCount)
	{
		int swapVal = GetRandomIntInRange(0, (int)m_deck.size() - 1);
		int swapVal2 = GetRandomIntInRange(0, (int)m_deck.size() - 1);
		Card* tempCard = nullptr;

		// swap cards around in array =============================================================================
		tempCard = m_deck[swapVal];
		m_deck[swapVal] = m_deck[swapVal2];
		m_deck[swapVal2] = tempCard;

		tempCard = nullptr;
	}	
}
