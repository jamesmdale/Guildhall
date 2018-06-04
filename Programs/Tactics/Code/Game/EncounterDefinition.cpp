#include "Game\EncounterDefinition.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


std::map<std::string, EncounterDefinition*> EncounterDefinition::s_encounterDefinitions;

EncounterDefinition::EncounterDefinition(const tinyxml2::XMLElement& element)
{
	m_id = ParseXmlAttribute(element, "id", m_id);

	const tinyxml2::XMLElement* mapInfoElement = element.FirstChildElement("MapInfo");
	if(mapInfoElement)
	{
		m_mapImageName = ParseXmlAttribute(*mapInfoElement, "mapImageName", m_mapImageName);
		m_mapMaxheight = ParseXmlAttribute(*mapInfoElement, "maxHeight", m_mapMaxheight);
	}

	const tinyxml2::XMLElement* actorsElement = element.FirstChildElement("Actors");
	if(actorsElement)
	{
		m_redFactionActors = ParseXmlAttribute(*actorsElement, "redFaction", m_redFactionActors);
		m_blueFactionActors = ParseXmlAttribute(*actorsElement, "blueFaction", m_blueFactionActors);
	}
}

void EncounterDefinition::Initialize(const std::string & filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		EncounterDefinition* newDef = new EncounterDefinition(*definitionNode);		
		s_encounterDefinitions.insert(std::pair<std::string, EncounterDefinition*>(newDef->m_id, newDef));
	}	
}
