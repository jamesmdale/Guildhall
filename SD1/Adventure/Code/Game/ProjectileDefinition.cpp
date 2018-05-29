#pragma once
#include "Game/ProjectileDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, ProjectileDefinition* > ProjectileDefinition:: s_projectileDefinitions;

ProjectileDefinition::ProjectileDefinition( const tinyxml2::XMLElement& element, Renderer& renderer) : EntityDefinition(element, renderer)
{		
	const tinyxml2::XMLElement* damageElement = element.FirstChildElement("Damage");
	if(damageElement)
	{
		m_damageAmount = ParseXmlAttribute(*damageElement, "amount", m_damageAmount);		
	}
}

void ProjectileDefinition::Initialize(const std::string& filePath, Renderer& renderer)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		ProjectileDefinition* newDef = new ProjectileDefinition(*definitionNode, renderer);		
		s_projectileDefinitions.insert(std::pair<std::string, ProjectileDefinition*>(newDef->m_name, newDef));
		//s_entityDefinitions.insert(std::pair<std::string, ProjectileDefinition*>(newDef->m_name, newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded projectile definitions!!!");
}
