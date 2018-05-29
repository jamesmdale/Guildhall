#include "Game\ActorDefinition.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


std::map<std::string, ActorDefinition*> ActorDefinition::s_actorDefinitions;

ActorDefinition::ActorDefinition(const tinyxml2::XMLElement& element)
{
	m_id = ParseXmlAttribute(element, "name", m_id);

	const tinyxml2::XMLElement* statsElement = element.FirstChildElement("Stats");
	if(statsElement) 
	{
		m_maxHealth = ParseXmlAttribute(*statsElement, "maxHealth", m_maxHealth);
		m_maxJumpHeight = ParseXmlAttribute(*statsElement, "maxJumpHeight", m_maxJumpHeight);
		m_maxActionSpeed = ParseXmlAttribute(*statsElement, "maxActionSpeed", m_maxActionSpeed);
		m_maxTilesMovement = ParseXmlAttribute(*statsElement, "maxTilesMovement", m_maxTilesMovement);
		m_height = ParseXmlAttribute(*statsElement, "height", m_height);
	}

	const tinyxml2::XMLElement* animSetElement = element.FirstChildElement("AnimSet");
	if(animSetElement)
	{
		m_animSetId = ParseXmlAttribute(*animSetElement, "id", m_animSetId);
	}
	
	const tinyxml2::XMLElement* abilitiesSetElement = element.FirstChildElement("Abilities");

	if(abilitiesSetElement)
	{
		for(const tinyxml2::XMLElement* definitionNode = abilitiesSetElement->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
		{
			AbilityStruct abilityStruct;

			abilityStruct.m_id = ParseXmlAttribute(*definitionNode, "id", abilityStruct.m_id);
			abilityStruct.m_range = ParseXmlAttribute(*definitionNode, "range", abilityStruct.m_range);	
			abilityStruct.m_valueRange = ParseXmlAttribute(*definitionNode, "valueRange", abilityStruct.m_valueRange);	
			abilityStruct.m_cost = ParseXmlAttribute(*definitionNode, "cost", abilityStruct.m_cost);	
			abilityStruct.m_delay = ParseXmlAttribute(*definitionNode, "delay", abilityStruct.m_delay);
			abilityStruct.m_areaOfEffectHorizontal = ParseXmlAttribute(*definitionNode, "areaHorizontal", abilityStruct.m_areaOfEffectHorizontal);	
			abilityStruct.m_areaOfEffectVertical = ParseXmlAttribute(*definitionNode, "areaVertical", abilityStruct.m_areaOfEffectVertical);	
			abilityStruct.m_type = ParseXmlAttribute(*definitionNode, "type", abilityStruct.m_type);

			TODO("This is a janky name. Come back and fix later to be more versatile.");
			abilityStruct.m_blockChanceBuff = ParseXmlAttribute(*definitionNode, "blockChanceBuff", abilityStruct.m_blockChanceBuff);	

			m_abilityStructs.push_back(abilityStruct);
		}		
	}
}

void ActorDefinition::Initialize(const std::string & filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		ActorDefinition* newDef = new ActorDefinition(*definitionNode);		
		s_actorDefinitions.insert(std::pair<std::string, ActorDefinition*>(newDef->m_id, newDef));
	}	
}

AbilityStruct ActorDefinition::GetAbilityStructById(std::string id)
{
	for(int abilityIndex = 0; abilityIndex < (int)m_abilityStructs.size(); abilityIndex++)
	{
		if(id == m_abilityStructs[abilityIndex].m_id)
		{
			return m_abilityStructs[abilityIndex];
		}
	}

	AbilityStruct emptyStruct;
	return emptyStruct;
}
