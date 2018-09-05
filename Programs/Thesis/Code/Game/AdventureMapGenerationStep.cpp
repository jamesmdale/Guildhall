#include "Game/AdventureMapGenerationStep.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

std::map< std::string, AdventureMapGenerationStep* > AdventureMapGenerationStep:: s_mapGenerationSteps;

AdventureMapGenerationStep::AdventureMapGenerationStep( const tinyxml2::XMLElement& element)
{	
	m_mapName = ParseXmlAttribute(element, "name", m_mapName);
	std::string mapDefinitionName = ParseXmlAttribute(element, "mapDefinition", std::string("default"));
	std::map<std::string, MapDefinition*>::iterator mapDefIterator = MapDefinition::s_definitions.find(mapDefinitionName);

	GUARANTEE_OR_DIE(mapDefIterator != MapDefinition::s_definitions.end(), "Missing map definition declared in AdventureDefinitions.xml for map: " + m_mapName);
	if(mapDefIterator != MapDefinition::s_definitions.end())
	{
		m_mapDefinition = mapDefIterator->second;
	}

	for(const tinyxml2::XMLElement* definitionNode = element.FirstChildElement("Portal"); definitionNode; definitionNode = definitionNode->NextSiblingElement("Portal"))
	{	
		AdventureMapPortalGenerationStep* newDefinition = new AdventureMapPortalGenerationStep(*definitionNode);

		m_mapPortals.insert(std::pair<std::string, AdventureMapPortalGenerationStep*>(std::string(m_mapName + "Portal" + std::to_string(m_mapPortals.size())), newDefinition));
	}

	/*TODO: Follow the above steps for actors and items and any other entity or object type that you choose. (MAP_Gen steps will include basic actors and items.
	this step is purely for ADVENTURE specific entities*/

	/*for(const tinyxml2::XMLElement* definitionNode = element.FirstChildElement("Actor"); definitionNode; definitionNode = definitionNode->NextSiblingElement("Actor"))
	{	
		AdventureMapActorGenerationStep* newDefinition = new AdventureMapActorGenerationStep(*definitionNode);

		m_mapPortals.insert(std::pair<std::string, AdventureMapActorGenerationStep*>(std::string(newDefinition->m_actorName), newDefinition));
	}

	for(const tinyxml2::XMLElement* definitionNode = element.FirstChildElement("Item"); definitionNode; definitionNode = definitionNode->NextSiblingElement("Item"))
	{	
		AdventureMapItemGenerationStep* newDefinition = new AdventureMapItemGenerationStep(*definitionNode);

		m_mapPortals.insert(std::pair<std::string, AdventureMapItemGenerationStep*>(std::string(newDefinition->m_ItemName), newDefinition));
	}*/

}
