#include "Game\Entity\Powers\Fireblast.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Gamestates\PlayingState.hpp"
#include "Game\Effects\DerivedEffects\TargetEffect.hpp"

Fireblast::Fireblast()
{
}


Fireblast::~Fireblast()
{
}

void Fireblast::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	//only allow left clicks on a minion if you are the controller
	if (playingState->m_activePlayer->m_playerId == m_controller->m_playerId)
	{
		if (m_isInputPriority == false)
		{
			if (m_usedThisTurn == false)
			{
				m_isInputPriority = true;
				m_isPositionLocked = false;
				UpdateSortLayer(g_sortLayerMax);

				TargetEffect* attackTarget = new TargetEffect((Widget)*this);
				AddEffectToEffectQueue(attackTarget);

				//cleanup
				attackTarget = nullptr;
			}
		}
	}	
}

void Fireblast::OnRightClicked()
{
	TODO("Display info");
}
