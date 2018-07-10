#include "Game\Entity\Powers\Fireblast.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Gamestates\PlayingState.hpp"

Fireblast::Fireblast()
{
}


Fireblast::~Fireblast()
{
}

void Fireblast::OnLeftClicked()
{
	//PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	////only allow left clicks on a minion if you are the controller
	//if (playingState->m_activePlayer->m_playerId == m_controller->m_playerId)
	//{
	//	if (m_isInputPriority == false)
	//	{
	//		if ((m_age > 0 || CheckForTag("charge")) && m_hasAttackedThisTurn == false && m_attack > 0)
	//		{
	//			m_isInputPriority = true;
	//			m_isPositionLocked = false;
	//			UpdateSortLayer(g_sortLayerMax);

	//			TargetEffect* attackTarget = new TargetEffect(this);
	//			AddEffectToEffectQueue(attackTarget);

	//			//cleanup
	//			attackTarget = nullptr;
	//		}
	//	}
	//}	
}

void Fireblast::OnRightClicked()
{
}
