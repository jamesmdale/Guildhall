#pragma once
#include "Game\Entity\Powers\HeroPower.hpp"

class Fireblast : public HeroPower
{
public:
	Fireblast();
	virtual ~Fireblast() override;

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;
};

