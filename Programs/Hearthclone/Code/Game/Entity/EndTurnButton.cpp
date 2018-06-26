#include "Game\Entity\EndTurnButton.hpp"
#include "Game\GameStates\PlayingState.hpp"

EndTurnButton::~EndTurnButton()
{
	m_playingState = nullptr;
}

void EndTurnButton::OnLeftClicked()
{
	//call end turn
	int i = 0;
}
