#pragma once
#include "Game/AdventureDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, AdventureDefinition* > AdventureDefinition:: s_definitions;

AdventureDefinition::AdventureDefinition( const tinyxml2::XMLElement& element )
{	
	m_name = ParseXmlAttribute(element, "name", m_name);
	m_title = ParseXmlAttribute(element, "title", m_title);
	const tinyxml2::XMLElement* startConditions = element.FirstChildElement("StartConditions");
	if(startConditions)
	{
		m_startMapName = ParseXmlAttribute(*startConditions, "startMap", m_startMapName);	
		m_startOnTileType = ParseXmlAttribute(*startConditions, "startOnTileType", m_startOnTileType);
	}

	//TODO: Could expand this later to include multiple different victory conditions or make a list of things you have to do. However robably easier to just make one and build around it.
	const tinyxml2::XMLElement* victoryConditions = element.FirstChildElement("VictoryConditions");
	if(victoryConditions)
	{
		m_haveDied = ParseXmlAttribute(*victoryConditions, "haveDied", m_haveDied);
	}

	for(const tinyxml2::XMLElement* definitionNode = element.FirstChildElement("Map"); definitionNode; definitionNode = definitionNode->NextSiblingElement("Map"))
	{	
		AdventureMapGenerationStep* newDefinition = new AdventureMapGenerationStep(*definitionNode);

		m_mapsToGenerate.insert(std::pair<std::string, AdventureMapGenerationStep*>(std::string(newDefinition->m_mapName), newDefinition));
	}
}

void AdventureDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		AdventureDefinition* newDef = new AdventureDefinition(*definitionNode);		
		s_definitions.insert(std::pair<std::string, AdventureDefinition*>(std::string(newDef->m_name), newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded adventure definitions!!!");
}
