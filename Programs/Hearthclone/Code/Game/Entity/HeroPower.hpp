#pragma once
#include "Engine\Core\Widget.hpp"
#include "Game\Actions\Action.hpp"

class Player;
class HeroPower : public Widget
{
public:
	HeroPower(){};
	//HeroPower(eClass classType);

	virtual ~HeroPower() override
	{
		m_controller = nullptr;
	}

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;

public:
	Player* m_controller = nullptr;
	int m_cost = 0;
	bool m_usedThisTurn = false;

	std::vector<ActionData*> m_powerActions; 
};