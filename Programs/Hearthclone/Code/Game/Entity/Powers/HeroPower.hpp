#pragma once
#include "Engine\Core\Widget.hpp"

class Player;
class HeroPower : public Widget
{
public:
	HeroPower(){};
	virtual ~HeroPower() override
	{
		m_controller = nullptr;
	}

public:
	Player* m_controller = nullptr;
	bool m_usedThisTurn = false;
};