#pragma once
#include "Game\Entity\Powers\HeroPower.hpp"

class Fireblast : HeroPower
{
public:
	Fireblast();
	virtual ~Fireblast() override;

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;
};

