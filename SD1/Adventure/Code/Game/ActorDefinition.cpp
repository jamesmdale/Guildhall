#pragma once
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

std::map< std::string, ActorDefinition* > ActorDefinition:: s_actorDefinitions;

ActorDefinition::ActorDefinition( const tinyxml2::XMLElement& element, Renderer& renderer ) : EntityDefinition(element, renderer)
{	
	m_faction = ParseXmlAttribute(element, "faction", m_faction);
	std::string attackableFactions = "";
	attackableFactions = ParseXmlAttribute(element, "attackableFactions", attackableFactions);
	m_attackableFactions = SplitStringOnCharacter(attackableFactions, ',');

	m_attackType = ParseXmlAttribute(element, "attackType", m_attackType);
	m_doesHaveActorToActorPhysics = ParseXmlAttribute(element, "doesHaveActorToActorPhysics", m_doesHaveActorToActorPhysics);
	m_mass = ParseXmlAttribute(element, "mass", m_mass);
	m_isUnpushable = ParseXmlAttribute(element, "isUnpushable", m_isUnpushable);

	if(m_isUnpushable)
	{
		m_mass = 99999.f; //something huge;
	}

	//parse Health	
	const tinyxml2::XMLElement* healthElement = element.FirstChildElement("Health");
	if(healthElement)
	{
		m_maxHealth = ParseXmlAttribute(*healthElement, "max", m_maxHealth);
		m_startHealth = ParseXmlAttribute(*healthElement, "start", m_startHealth);
	}

	//TODO: Add more stats here as you see fit
	const tinyxml2::XMLElement* statsElement = element.FirstChildElement("Stats");
	if(statsElement)
	{
		m_minStats.stats[STAT_DAMAGE]  = ParseXmlAttribute(*statsElement, "minDamage", m_minStats.stats[STAT_DAMAGE]);
		m_minStats.stats[STAT_STRENGTH]  = ParseXmlAttribute(*statsElement, "minStrength", m_minStats.stats[STAT_STRENGTH]);
		m_minStats.stats[STAT_ARMOR]  = ParseXmlAttribute(*statsElement, "minArmor", m_minStats.stats[STAT_ARMOR]);
		m_minStats.stats[STAT_MELEE_RANGE]  = ParseXmlAttribute(*statsElement, "minMeleeRange", m_minStats.stats[STAT_MELEE_RANGE]);

		m_maxStats.stats[STAT_DAMAGE]  = ParseXmlAttribute(*statsElement, "maxDamage", m_maxStats.stats[STAT_DAMAGE]);
		m_maxStats.stats[STAT_STRENGTH]  = ParseXmlAttribute(*statsElement, "maxStrength", m_maxStats.stats[STAT_STRENGTH]);
		m_maxStats.stats[STAT_ARMOR]  = ParseXmlAttribute(*statsElement, "maxArmor", m_maxStats.stats[STAT_ARMOR]);
		m_maxStats.stats[STAT_MELEE_RANGE]  = ParseXmlAttribute(*statsElement, "maxMeleeRange", m_maxStats.stats[STAT_MELEE_RANGE]);
	}
}

void ActorDefinition::Initialize(const std::string& filePath, Renderer& renderer)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		ActorDefinition* newDef = new ActorDefinition(*definitionNode, renderer);		
		s_actorDefinitions.insert(std::pair<std::string, ActorDefinition*>(newDef->m_name, newDef));
		//s_entityDefinitions.insert(std::pair<std::string, ActorDefinition*>(newDef->m_name, newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded actor definitions!!!");
}
