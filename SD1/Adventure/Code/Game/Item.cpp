#include "Game/Item.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"

Item::Item(ItemDefinition* itemDef) : Entity((EntityDefinition*)itemDef)
{
	m_itemDef = itemDef;

	m_remainingUses = m_itemDef->m_allowedUses;
}