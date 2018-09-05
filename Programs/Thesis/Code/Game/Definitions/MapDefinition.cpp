#include "Game\Definitions\MapDefinition.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Game\Map\MapGenStep.hpp"

std::map< std::string, MapDefinition* > MapDefinition:: s_definitions;

MapDefinition::MapDefinition( const tinyxml2::XMLElement& element )
{	
	m_name = ParseXmlAttribute(element, "name", m_name);
	m_width = ParseXmlAttribute(element, "width", m_width);
	m_height = ParseXmlAttribute(element, "height", m_height);

	std::string defaultTileName = "default";
	std::string tileNameParsedString = ParseXmlAttribute(element, "defaultTile", defaultTileName);
	GUARANTEE_OR_DIE(tileNameParsedString != "default", "No default tile set or incorrect matching name from known tile definitions");
	m_defaultTile = TileDefinition::s_definitions[tileNameParsedString];

	const tinyxml2::XMLElement* pRoot = element.FirstChildElement();
	m_iterations = ParseXmlAttribute(*pRoot, "iterations", m_iterations);
	m_chanceToRun = ParseXmlAttribute(*pRoot, "chanceToRun", m_chanceToRun);

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{	
		MapGenStep* newStep = MapGenStep::CreateMapGenStep(*definitionNode);		
		m_genSteps.push_back(newStep);
	}
}

void MapDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		MapDefinition* newDef = new MapDefinition(*definitionNode);		
		s_definitions.insert(std::pair<std::string, MapDefinition*>(std::string(newDef->m_name), newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded map definitions!!!");
}