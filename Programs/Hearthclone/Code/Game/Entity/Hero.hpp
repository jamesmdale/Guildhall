#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\HeroPower.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\GameCommon.hpp"

class Hero : public Character
{
public:
	Hero();
	Hero(HeroDefinition* definition, ePlayerType controller) : Character("")
	{
		m_heroDefinition = definition;
		m_controller = controller;
		m_name = m_heroDefinition->m_heroName;

		m_heroPower = new HeroPower();
		m_heroPower->m_name = m_heroDefinition->m_powerName;
		m_heroPower->m_cost = m_heroDefinition->m_powerCost;
		m_heroPower->m_actions = m_heroDefinition->m_actionPowers;
		m_heroPower->m_controller = m_controller;

		//m_heroPower->Initialize();

		m_type = CHARACTER_TYPE_HERO;
	}

	virtual ~Hero() override
	{
		delete(m_heroPower);
		m_heroPower = nullptr;
	}

	virtual void Initialize() override;
	virtual void RefreshRenderables() override;

public:
	int m_armor = 0;
	HeroPower* m_heroPower = nullptr;	
	HeroDefinition* m_heroDefinition = nullptr;

	Texture* m_heroImage = nullptr;
};

