#include "Game\Actions\Action.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Minion.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Effects\DerivedEffects\DrawEffect.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeMinionsEffect.hpp"
#include "Game\Board.hpp"
#include "Game\TurnStates\TurnStateManager.hpp"
#include "Game\Effects\DerivedEffects\TurnChangeEffect.hpp"
#include "Game\Effects\DerivedEffects\AttackEffect.hpp"
#include "Game\Effects\DerivedEffects\DeathEffect.hpp"

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
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("target")->second);
	int drawAmount = atoi(parameters.find("amount")->second.c_str());

	// process function =============================================================================
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* targetPlayer = nullptr;

	AABB2 deckQuad;
	AABB2 handQuad;
	if (playerType == ENEMY_PLAYER_TYPE)
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

	//process each draw
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

		float effectTime = 1.f / drawAmount;

		DrawEffect* drawEffect = new DrawEffect(card, effectTime, targetPlayer->m_playerId, deckQuad.GetCenter(), endPosition, Vector2(0.1f, 0.1f));
		AddEffectToEffectQueue(drawEffect);

		card = nullptr;
		drawEffect = nullptr;
	}

	targetPlayer = nullptr;
	gameState = nullptr;
}

void AttackAction(const std::map<std::string, std::string>& parameters)
{
	int attackerIndex = ConvertStringToInt(parameters.find("attackerId")->second);
	int targetIndex = ConvertStringToInt(parameters.find("targetId")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Character* attackingCharacter = gameState->GetCharacterById(attackerIndex);
	Character* targetCharacter =  gameState->GetCharacterById(targetIndex);

	std::map<std::string, std::string> damageParameters;

	//deal damage to target
	if (targetCharacter->m_attack > 0)
	{
		damageParameters = { {"targetId", Stringf("%i", attackerIndex)}, {"amount", Stringf("%i", targetCharacter->m_attack)} };
		AddActionToRefereeQueue("damage", damageParameters);
	}
		
	//deal damage to attacking character
	if (attackingCharacter->m_attack > 0)
	{
		damageParameters = { {"targetId", Stringf("%i", targetIndex)}, {"amount", Stringf("%i", attackingCharacter->m_attack)} };
		AddActionToRefereeQueue("damage", damageParameters);
	}

	//add attack effect to queue
	AttackEffect* effect = new AttackEffect(attackingCharacter, 0.5f, attackingCharacter->m_lockPosition, targetCharacter->m_lockPosition);
	AddEffectToEffectQueue(effect);

	TODO("damage queue");
	 
	TODO("Damage triggers");
	TODO("Death triggers");
	TODO("Attack Effect");

	attackingCharacter->m_isInputPriority = false;

	targetCharacter = nullptr;	
	attackingCharacter = nullptr;
	gameState = nullptr;
}

void CastFromHandAction(const std::map<std::string, std::string>& parameters)
{
	// Get Parameters =============================================================================
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("target")->second);
	int cardIndex = ConvertStringToInt(parameters.find("handIndex")->second);
	Vector2 battlefieldLocation = ConvertStringToVector2(parameters.find("newLocation")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* playerCasting = nullptr;
	
	if (playerType == SELF_PLAYER_TYPE)
		playerCasting = gameState->m_player;
	else
		playerCasting = gameState->m_enemyPlayer;

	Card* cardToCast = playerCasting->m_hand[cardIndex];	

	// Process Function =============================================================================	
	if (cardToCast->m_definition->m_type == MINION_TYPE)
	{
		TODO("Cast triggers here");

		//if we have max minions in play or not enough mana to cast, we can't cast card. return.
		if ((int)playerCasting->m_minions.size() >= g_maxMinionCount || playerCasting->m_manaCount < cardToCast->m_definition->m_cost)
		{
			cardToCast->OnRightClicked(); //handles input priority

			//cleanup and return
			gameState = nullptr;
			playerCasting = nullptr;
			cardToCast = nullptr;
			return;
		}

		//set card position
		cardToCast->m_transform2D->SetLocalPosition(battlefieldLocation);
		cardToCast->m_lockPosition = battlefieldLocation;

		playerCasting->m_manaCount -= cardToCast->m_cost;

		Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();
		
		int castPosition = 0;

		//determine minion spawn location by reording array position (minions on battlefield are ordered left to right)
		for (int minionIndex = 0; minionIndex < (int)playerCasting->m_minions.size(); ++minionIndex)
		{
			Vector2 minionPosition = playerCasting->m_minions[minionIndex]->m_transform2D->GetLocalPosition();
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
		std::vector<Minion*>::iterator minionIterator = playerCasting->m_minions.begin();
		playerCasting->m_minions.insert(minionIterator + castPosition, newMinion);

		//remove card from hand
		playerCasting->RemoveCardFromHand(cardIndex);

		newMinion->RefreshRenderables();
		
		//update dynamic renderables
		playerCasting->UpdateBoardLockPositions();
		playerCasting->UpdateHandLockPositions();
		playerCasting->UpdateDeckCount();

		gameState->m_gameBoard->RefreshPlayerManaWidget();

		newMinion->m_transform2D->SetLocalPosition(newMinion->m_lockPosition);

		if ((int)playerCasting->m_hand.size() > 0)
		{
			float timeForEffect = 0.25f / (float)playerCasting->m_hand.size();
			ReorganizeHandEffect* handEffect = new ReorganizeHandEffect(timeForEffect, playerType);
			AddEffectToEffectQueue(handEffect);

			handEffect = nullptr;
		}		

		if ((int)playerCasting->m_minions.size() > 0)
		{
			float timeForEffect = 0.25f / (float)playerCasting->m_minions.size();
			ReorganizeMinionsEffect* minionEffect = new ReorganizeMinionsEffect(timeForEffect, playerType);
			AddEffectToEffectQueue(minionEffect);

			minionEffect = nullptr;
		}		
		
		newMinion = nullptr;
	}

	//if (cardToCast->m_definition->m_type == SPELL_TYPE)
	//{

	//}
	TODO("Weapon type");

	// cleanup =============================================================================

	cardToCast = nullptr;
	playerCasting = nullptr;
	gameState = nullptr;
}

void EndTurnAction(const std::map<std::string, std::string>& parameters)
{
	UNUSED(parameters);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	TODO("Handle triggers");

	gameState->m_turnStateManager->TransitionToState(END_OF_TURN_PLAY_STATE);

	gameState = nullptr;
}

void StartTurnAction(const std::map<std::string, std::string>& parameters)
{
	UNUSED(parameters);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	TODO("Handle triggers");

	TurnChangeEffect* turnChangeEffect = new TurnChangeEffect(gameState->m_activePlayer->m_playerId, 1.5f, gameState->m_renderScene2D);

	AddEffectToEffectQueue(turnChangeEffect);

	//cleanup

	turnChangeEffect = nullptr;
	gameState = nullptr;
}

void DamageAction(const std::map<std::string, std::string>& parameters)
{
	int targetId = ConvertStringToInt(parameters.find("targetId")->second);
	int damageAmount = ConvertStringToInt(parameters.find("amount")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Character* targetCharacter = gameState->GetCharacterById(targetId);
	targetCharacter->m_health -= damageAmount;

	if (targetCharacter->m_health <= 0)
	{
		if (targetCharacter->CheckForTag("minion"))
		{
			DeathEffect* deathEffect = new DeathEffect((Minion*)targetCharacter, 0.5f);
			AddEffectToEffectQueue(deathEffect);
		}			
	}

	targetCharacter->RefreshRenderables();
	targetCharacter->UpdateRenderable2DFromTransform();		

	targetCharacter = nullptr;
	gameState = nullptr;
}

