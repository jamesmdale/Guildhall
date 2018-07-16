#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\HeroPower.hpp"
#include "Game\GameCommon.hpp"

class Hero : public Character
{
public:
	Hero()
	{
		m_type = CHARACTER_TYPE_HERO;
	}

	Hero(const eClass classType) : Character("")
	{
		switch (classType)
		{
		//case DRUID_CLASS:
		//	//color = g_druidClassColor;
		//	break;
		//case HUNTER_CLASS:
		//	//color = g_hunterClassColor;
		//	break;
		case MAGE_CLASS:
			//m_heroPower = new HeroPower();
			break;
		/*case PALADIN_CLASS:
			color = g_paladinClassColor;
			break;
		case PRIEST_CLASS:
			color = g_priestClassColor;
			break;
		case ROGUE_CLASS:
			color = g_rogueClassColor;
			break;
		case SHAMAN_CLASS:
			color = g_shamanClassColor;
			break;
		case WARLOCK_CLASS:
			color = g_warlockClassColor;
			break;
		case WARRIOR_CLASS:
			color = g_warriorClassColor;
			break;
		case NEUTRAL_CLASS:
			color = g_neutralClassColor;
			break;*/
		}

		m_type = CHARACTER_TYPE_HERO;
	}

	virtual ~Hero() override
	{
		delete(m_heroPower);
		m_heroPower = nullptr;
	}

public:
	int m_armor = 0;
	HeroPower* m_heroPower = nullptr;	
};

