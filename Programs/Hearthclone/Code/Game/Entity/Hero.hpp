#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Powers\Fireblast.hpp"

class Hero : public Character
{
public:
	Hero();
	Hero(const std::string& name) : Character(name)
	{
		//board creation
	}

	virtual ~Hero() override;



public:
	int m_armor = 0;
	HeroPower* m_heroPower = nullptr;

	
};

