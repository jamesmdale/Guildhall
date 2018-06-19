#include "Game\Entity\Player.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Definitions\CardDefinition.hpp"
#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\Entity\Hero.hpp"


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
		cardToAdd->m_controller = m_playerId;
		cardToAdd->m_renderScene = m_gameState->m_renderScene2D;
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

void Player::UpdateHandLockPositions()
{
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Board* board = gameState->m_gameBoard;

	AABB2 handQuad;
	if (m_playerId == 0) //if player is self
	{
		handQuad = board->m_playerHandQuad;
	}
	if (m_playerId == 1) //if player is enemy
	{
		handQuad = board->m_enemyHandQuad;
	}

	float handDockCenterHeight = handQuad.maxs.y - ((handQuad.maxs.y - handQuad.mins.y) * 0.5f);
	float handDockWidthPerCard = (handQuad.maxs.x - handQuad.mins.x) / (float)(	g_maxHandSize + 1); // + 1 because we include deck image

	for (int cardIndex = 0; cardIndex < (int)m_hand.size(); ++cardIndex)
	{	
		if (m_hand[cardIndex]->m_isRendering == false)
		{
			m_hand[cardIndex]->m_renderScene = g_currentState->m_renderScene2D;
			m_hand[cardIndex]->RefreshCardRenderables();
		}
		
		m_hand[cardIndex]->m_lockPosition = Vector2(handDockWidthPerCard * (cardIndex + 1), handDockCenterHeight);	
	}

	// cleanup =============================================================================
	gameState = nullptr;
	board = nullptr;
}

void Player::UpdateBoardLockPositions()
{
}
