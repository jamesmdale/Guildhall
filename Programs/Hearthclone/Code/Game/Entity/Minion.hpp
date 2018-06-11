#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"

class Minion : public Character
{
public:
	Minion();
	Minion(const std::string& name) : Character(name)
	{
		//board creation
	}

	virtual ~Minion() override;

public:
	//minion cards don't go to the graveyard as soon as they are put in play. Therefore we need to know if we have a card to put in the graveyard when this
	Card* m_cardReference = nullptr; 
};

