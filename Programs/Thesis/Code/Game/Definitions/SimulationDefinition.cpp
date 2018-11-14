#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Definitions\SimulationDefinition.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\MapGenStep.hpp"

std::map< std::string, SimulationDefinition*> SimulationDefinition:: s_simulationDefinitions;

//  =============================================================================
SimulationDefinition::SimulationDefinition( const tinyxml2::XMLElement& element )
{	
	m_name = ParseXmlAttribute(element, "name", m_name);
	m_mapName = ParseXmlAttribute(element, "mapName", m_mapName);
	m_numAgents = ParseXmlAttribute(element, "numAgents", m_numAgents);
	m_numArmories = ParseXmlAttribute(element, "numArmories", m_numArmories);
	m_numLumberyards = ParseXmlAttribute(element, "numLumberyards", m_numLumberyards);
	m_numMedStations = ParseXmlAttribute(element, "numMedStations", m_numMedStations);
	m_bombardmentRatePerSecond = ParseXmlAttribute(element, "bombardmentRatePerSecond", m_bombardmentRatePerSecond);
	m_threatRatePerSecond = ParseXmlAttribute(element, "threatRatePerSecond", m_threatRatePerSecond);
	m_startingThreat = ParseXmlAttribute(element, "startingThreat", m_startingThreat);	
	m_totalProcessingTimeInSeconds = ParseXmlAttribute(element, "processTimerInSeconds", m_totalProcessingTimeInSeconds);

	m_mapDefinition = MapDefinition::GetMapDefinitionByName(m_mapName);
}

//  =============================================================================
void SimulationDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		SimulationDefinition* newDef = new SimulationDefinition(*definitionNode);		
		s_simulationDefinitions.insert(std::pair<std::string, SimulationDefinition*>(std::string(newDef->m_name), newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded map definitions!!!");
}

//  =============================================================================
SimulationDefinition* SimulationDefinition::GetSimulationByName(const std::string& definitionName)
{
	//if none are loaded, error out
	ASSERT_OR_DIE(s_simulationDefinitions.size() > 0, "ERROR: NO SIMULATION DEFINITIONS LOADED");

	//if they pass in an empty string, get the first entry
	if (IsStringNullOrEmpty(definitionName))
	{
		return s_simulationDefinitions.begin()->second;
	}

	auto definition = s_simulationDefinitions.find(definitionName);

	//if the definition is not found, error out 
	ASSERT_OR_DIE(definition != s_simulationDefinitions.end(), "ERROR: INVALID SIMULATION NAME");

	return definition->second;	
}
