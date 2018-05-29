#pragma once
#include "Game/Entity.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/RaycastResult.hpp"

class Item : public Entity
{
public:
	Item(ItemDefinition* itemDef);
	~Item(){};

	//void Render();
public:
	bool m_isEquipped = false;
	float m_currentUseCooldown = 0.f; //TODO could have an update statement here later.  Used for consummables and items with activated abilities. currently just set this to 0 so it's always ready
	int m_remainingUses = 0;
	ItemDefinition* m_itemDef = nullptr;

};
