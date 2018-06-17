#include "Game\Actions\Action.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\StringUtils.hpp"

// actions =============================================================================

//template
/*	TEMPLATE
void DoStuff(const std::map<std::string, std::string>& parameters)
{
// Get Parameters =============================================================================
std::string thing1 = parameters.find("thing")->second;
int thing2 = atoi(parameters.find("thing2")->second.c_str());

// Process Function =============================================================================		
}
*/

void DrawAction(const std::map<std::string, std::string>& parameters)
{
	// get parameters =============================================================================
	std::string player = parameters.find("target")->second;
	int drawAmount = atoi(parameters.find("amount")->second.c_str());

	// process function =============================================================================
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* targetPlayer = nullptr;

	AABB2 deckQuad;
	AABB2 handQuad;
	if (player == "enemy")
	{
		targetPlayer = gameState->m_enemyPlayer;	
		deckQuad = gameState->m_gameBoard->m_enemyDeckQuad;
		handQuad = gameState->m_gameBoard->m_enemyHandQuad;
	}
	else
	{
		targetPlayer = gameState->m_player;
		deckQuad = gameState->m_gameBoard->m_playerDeckQuad;
		handQuad = gameState->m_gameBoard->m_playerHandQuad;
	}

	for (int drawIndex = 0; drawIndex < drawAmount; ++drawIndex)
	{
		//add card to hand. remove card from deck
		Card* card = targetPlayer->m_deck[targetPlayer->m_deck.size() - 1];

		targetPlayer->m_hand.push_back(card);

		targetPlayer->m_deck[targetPlayer->m_deck.size() - 1] = nullptr;
		targetPlayer->m_deck.pop_back();

		//update card renderable at starting position before effect
		card->RefreshCardRenderables();
		card->m_transform2D->SetLocalPosition(deckQuad.GetCenter());
		card->m_transform2D->SetLocalScale(Vector2(0.1f, 0.1f));

		//get cards final location
		float handDockCenterHeight = handQuad.maxs.y - ((handQuad.maxs.y - handQuad.mins.y) * 0.5f);
		float handDockWidthPerCard = (handQuad.maxs.x - handQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

		Vector2 endPosition = Vector2(handDockWidthPerCard * (targetPlayer->m_hand.size()), handDockCenterHeight);

		//initiate draw effect
		std::map<std::string, std::string> effectParams;
		effectParams.insert(std::pair<std::string, std::string>("totalEffectTime", Stringf("%f", 1.f)));
		effectParams.insert(std::pair<std::string, std::string>("playerID", Stringf("%i", targetPlayer->m_playerId)));
		effectParams.insert(std::pair<std::string, std::string>("startPosition", Stringf("%f,%f", deckQuad.GetCenter().x, deckQuad.GetCenter().y)));
		effectParams.insert(std::pair<std::string, std::string>("endPosition", Stringf("%f,%f", endPosition.x,endPosition.y)));
		effectParams.insert(std::pair<std::string, std::string>("startScale", Stringf("%f,%f", 0.1f, 0.1f)));

		AddEffectToEffectQueue("draw_effect", card, effectParams);

		card = nullptr;
	}

	//targetPlayer->UpdateDeckCount();
	//targetPlayer->UpdateHandLockPositions();


	targetPlayer = nullptr;
	gameState = nullptr;
}

void AttackAction(const std::map<std::string, std::string>& parameters)
{
	// Get Parameters =============================================================================
	std::string attackTarget = parameters.find("target")->second;

	// Process Function =============================================================================		

}