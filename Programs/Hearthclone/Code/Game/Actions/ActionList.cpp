#include "Game\Actions\Action.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Minion.hpp"
#include "Engine\Input\InputSystem.hpp"

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

void CastFromHandAction(const std::map<std::string, std::string>& parameters)
{
	// Get Parameters =============================================================================
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("target")->second);
	int cardIndex = ConvertStringToInt(parameters.find("handIndex")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* player = nullptr;
	
	if (playerType == SELF_PLAYER_TYPE)
		player = gameState->m_player;
	else
		player = gameState->m_enemyPlayer;

	Card* cardToCast = player->m_hand[cardIndex];	

	// Process Function =============================================================================	
	if (cardToCast->m_definition->m_type == MINION_TYPE)
	{
		TODO("Cast triggers here");

		if ((int)player->m_minions.size() > 10)
		{
			TODO("Don't allow them to cast. BREAK");
		}

		Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetMouseClientPosition();
		
		int castPosition = 0;

		//determine minion spawn location by reording array position (minions on battlefield are ordered left to right)
		for (int minionIndex = 0; minionIndex < (int)player->m_minions.size(); ++minionIndex)
		{
			Vector2 minionPosition = player->m_minions[minionIndex]->m_transform2D->GetLocalPosition();
			if (mousePosition.x < minionPosition.x)
			{
				castPosition = minionIndex;
				break;
			}
		}

		//create minion from card
		Minion* newMinion = new Minion(cardToCast);
		newMinion->m_renderScene = g_currentState->m_renderScene2D;
		
		//add minon to battlefield
		std::vector<Minion*>::iterator minionIterator = player->m_minions.begin();
		player->m_minions.insert(minionIterator + castPosition, newMinion);

		//remove card from hand
		player->RemoveCardFromHand(cardIndex);

		newMinion->RefreshRenderables();
		
		player->UpdateBoardLockPositions();
		player->UpdateHandLockPositions();
		player->UpdateDeckCount();

		newMinion->m_transform2D->SetLocalPosition(newMinion->m_lockPosition);

		newMinion = nullptr;
	}

	//if (cardToCast->m_definition->m_type == SPELL_TYPE)
	//{

	//}
	TODO("Weapon type");

	// cleanup =============================================================================

	cardToCast = nullptr;
	player = nullptr;
	gameState = nullptr;
}