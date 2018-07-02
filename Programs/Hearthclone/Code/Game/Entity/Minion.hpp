#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Player.hpp"

class Minion : public Character
{
public:
	Minion();
	Minion(const std::string& name) : Character(name)
	{
		//board creation
	}
	Minion(const Card* fromCard);
	virtual ~Minion() override;

	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	void RefreshRenderables();
	Vector2 GetMinionDimensions();

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;

	bool CheckForTag(const std::string& tagName);

public:
	//minion cards don't go to the graveyard as soon as they are put in play. Therefore we need to know if we have a card to put in the graveyard when this
	const Card* m_cardReference = nullptr; 

	Texture* m_minionLayoutImage = nullptr;
	Texture* m_minionImage = nullptr;

	ePlayerType m_controller;

	std::vector<std::string> m_tags;

	int m_age = 0;
	bool m_hasAttackedThisTurn = false;
};

