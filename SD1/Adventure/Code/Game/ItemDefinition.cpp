#pragma once
#include "Game/ItemDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, ItemDefinition* > ItemDefinition:: s_itemDefinitions;

ItemDefinition::ItemDefinition( const tinyxml2::XMLElement& element, Renderer& renderer ) : EntityDefinition(element, renderer)
{	
	std::string slotName = "";
	slotName = ParseXmlAttribute(element, "slot", slotName);
	MapSlotNameToEnum(slotName);

	m_itemType = ParseXmlAttribute(element, "type", m_itemType);
	m_attackType = ParseXmlAttribute(element, "type", m_attackType);

	const tinyxml2::XMLElement* abilityElement = element.FirstChildElement("ActivatedAbility");
	if(abilityElement)
	{
		m_effectedStatModifier = ParseXmlAttribute(*abilityElement, "stat", m_effectedStatModifier);
		m_effectAmount = ParseXmlAttribute(*abilityElement, "amount", m_effectAmount);
		m_allowedUses = ParseXmlAttribute(*abilityElement, "uses", m_allowedUses);
		m_useCooldown = ParseXmlAttribute(*abilityElement, "coolDown", m_useCooldown);
	}

	const tinyxml2::XMLElement* statsElement = element.FirstChildElement("Stats");
	if(statsElement)
	{
		m_stats.stats[STAT_DAMAGE]  = ParseXmlAttribute(*statsElement, "damage", m_stats.stats[STAT_DAMAGE]);
		m_stats.stats[STAT_STRENGTH]  = ParseXmlAttribute(*statsElement, "strength", m_stats.stats[STAT_STRENGTH]);
		m_stats.stats[STAT_ARMOR]  = ParseXmlAttribute(*statsElement, "armor", m_stats.stats[STAT_ARMOR]);
	}
	

	//TODO: Add conditions for stats.
}

void ItemDefinition::Initialize(const std::string& filePath, Renderer& renderer)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		ItemDefinition* newDef = new ItemDefinition(*definitionNode, renderer);		
		s_itemDefinitions.insert(std::pair<std::string, ItemDefinition*>(newDef->m_name, newDef));	
	}	

	//debugger notification
	DebuggerPrintf("Loaded item definitions!!!");
}

void ItemDefinition::MapSlotNameToEnum(std::string slotName)
{
	if(slotName == "head")	
		m_slot = HEAD_SLOT;
	else if(slotName == "torso")	
		m_slot = TORSO_SLOT;
	else if(slotName == "arm")	
		m_slot = ARM_SLOT;
	else if(slotName == "legs")	
		m_slot = LEGS_SLOT;
	else if(slotName == "feet")	
		m_slot = FEET_SLOT;
	else if(slotName == "belt")	
		m_slot = BELT_SLOT;
	else if(slotName == "weapon")	
		m_slot = WEAPON_SLOT;	
}
