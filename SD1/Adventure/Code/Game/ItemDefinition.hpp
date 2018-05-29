#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Stats.hpp"

enum EquipmentSlot
{
	NONE_SLOT,
	HEAD_SLOT, 
	TORSO_SLOT,	
	ARM_SLOT,
	LEGS_SLOT,
	BELT_SLOT,
	FEET_SLOT,
	WEAPON_SLOT,
	NUM_EQUIPMENT_SLOTS
};

class ItemDefinition : EntityDefinition
{
public:
	explicit ItemDefinition( const tinyxml2::XMLElement& element, Renderer& renderer);
	static void Initialize(const std::string& filePath, Renderer& renderer);
	void MapSlotNameToEnum(std::string slotName);
public: 
	std::string m_itemType = "default";
	std::string m_attackType = "cast";
	Stats m_stats;

	//list of tile definition member variables
	EquipmentSlot m_slot = NONE_SLOT; //TODO: Items in NONE_SLOT go straight into inventory.
	
	//for consumables
	std::string m_effectedStatModifier = "";
	float m_effectAmount = 0.f;
	int m_allowedUses = 0;
	float m_useCooldown = 0.f;

	static std::map< std::string, ItemDefinition* >	s_itemDefinitions;
};