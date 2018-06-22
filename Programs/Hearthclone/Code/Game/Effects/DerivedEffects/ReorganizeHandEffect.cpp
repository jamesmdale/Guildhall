#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\Entity\Card.hpp"

int currentHandIndex = 0;
Vector2 startPosition;
Vector2 endPosition;

ReorganizeHandEffect::ReorganizeHandEffect(float effectTime, ePlayerType player)
{
	m_totalEffectTime = effectTime;
	m_playerId = player;

	Initialize();
}

ReorganizeHandEffect::~ReorganizeHandEffect() 
{
	m_player = nullptr;
	m_gameState = nullptr;
}

void ReorganizeHandEffect::Update(float deltaSeconds)
{
	// if this is the first time in the loop we need to initialize values =============================================================================
	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
	}
	
	if (m_gameState == nullptr)
	{
		m_gameState = (PlayingState*)g_currentState;
	}
	if (m_player = nullptr)
	{
		if (m_playerId == SELF_PLAYER_TYPE)
		{
			m_player = m_gameState->m_player;
		}
		else if (m_playerId == ENEMY_PLAYER_TYPE)
		{
			m_player = m_gameState->m_enemyPlayer;
		}
	}

	// process function =============================================================================
	AABB2 handQuad = m_gameState->m_gameBoard->m_playerHandQuad;
	Card* card = m_player->m_hand[currentHandIndex];

	//get cards final location
	float handDockCenterHeight = handQuad.maxs.y - ((handQuad.maxs.y - handQuad.mins.y) * 0.5f);
	float handDockWidthPerCard = (handQuad.maxs.x - handQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

	endPosition = Vector2((handDockWidthPerCard * currentHandIndex), handDockCenterHeight);

	Interpolate(startPosition, endPosition, SmoothStop3((float)m_stopWatch->GetNormalizedElapsedTimeInSeconds()))
}

void ReorganizeHandEffect::Initialize()
{
	m_stopWatch = new Stopwatch(GetMasterClock());
	m_stopWatch->SetTimer(m_totalEffectTime);

	m_gameState = (PlayingState*)g_currentState;

	if (m_playerId == SELF_PLAYER_TYPE)
	{
		m_player = m_gameState->m_player;
	}
	else if (m_playerId == ENEMY_PLAYER_TYPE)
	{
		m_player = m_gameState->m_enemyPlayer;
	}

	AABB2 handQuad = m_gameState->m_gameBoard->m_playerHandQuad;
	Card* card = m_player->m_hand[currentHandIndex];

	//get cards final location
	float handDockCenterHeight = handQuad.maxs.y - ((handQuad.maxs.y - handQuad.mins.y) * 0.5f);
	float handDockWidthPerCard = (handQuad.maxs.x - handQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

	startPosition = m_player->m_hand[currentHandIndex]->m_transform2D->GetWorldPosition();
	endPosition = Vector2((handDockWidthPerCard * currentHandIndex), handDockCenterHeight);
}