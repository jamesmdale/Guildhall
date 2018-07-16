#include "Game\Entity\HeroPower.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Gamestates\PlayingState.hpp"
#include "Game\Effects\DerivedEffects\AbilityTargetEffect.hpp"
#include "Game\Actions\Action.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Definitions\HeroDefinition.hpp"


//  =========================================================================================
//HeroPower::HeroPower(eClass classType)
//{
//	//HeroDefinition* heroDefinition = HeroDefinition::GetDefinitionByName(classType);
//}

//  =========================================================================================
void HeroPower::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	//only allow left clicks on a minion if you are the controller
	if (playingState->m_activePlayer->m_playerId == m_controller->m_playerId)
	{
		if (m_isInputPriority == false && m_usedThisTurn == false)
		{
			m_isInputPriority = true;
			m_isPositionLocked = false;
			UpdateSortLayer(g_sortLayerMax);

			std::map<std::string, std::string> params = { {"action", "damage"}, {"targetId", ""}, {"amount", Stringf("%i", 2)} };

			AbilityTargetEffect* attackTarget = new AbilityTargetEffect(this);

			attackTarget->m_actions.push_back(&ActionData("hero_power", params));

			AddEffectToEffectQueue(attackTarget);

			//cleanup
			attackTarget = nullptr;
		}
	}
}

//  =========================================================================================
void HeroPower::OnRightClicked()
{
	TODO("Display info");
}
