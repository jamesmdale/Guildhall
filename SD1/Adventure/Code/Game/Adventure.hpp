#pragma once
#include "Game/Entity.hpp"
#include "Game/AdventureDefinition.hpp"

class Adventure
{
public:
	Adventure(AdventureDefinition* adventureDef);
	~Adventure(){};
	//void Render();
public:
	AdventureDefinition* m_adventureDef = nullptr;
};
