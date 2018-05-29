#include "Game/MapGenStep.hpp"
#include "Game/MapGenStep_FillAndEdge.hpp"
#include "Game/MapGenStep_FromFile.hpp"
#include "Game/MapGenStep_Mutate.hpp"
#include "game/MapGenStep_SpawnActor.hpp"
#include "Game/MapGenStep_CellularAutomata.hpp"
#include "game/MapGenStep_SpawnItem.hpp"

MapGenStep::MapGenStep( const tinyxml2::XMLElement& generationStepElement)
{
	m_name = generationStepElement.Name();
	m_ifTags = ParseXmlAttribute( generationStepElement, "ifTags", m_ifTags );
	m_setTags = ParseXmlAttribute( generationStepElement, "setTags", m_ifTags );
}

MapGenStep* MapGenStep::CreateMapGenStep( const tinyxml2::XMLElement& genStepXmlElement )
{
	std::string elementName = genStepXmlElement.Name();	
	if(elementName == "FillAndEdge")
	{
		return new MapGenStep_FillAndEdge(genStepXmlElement);
	}
	if(elementName == "FromFile")
	{
		return new MapGenStep_FromFile(genStepXmlElement);
	}
	if(elementName == "Mutate")
	{
		return new MapGenStep_Mutate(genStepXmlElement);
	}
	//TODO: Add SpawnActor
	if(elementName == "SpawnActor")
	{
		return new MapGenStep_SpawnActor(genStepXmlElement);
	}
	//TODO: Add SpawnItem
	if(elementName == "SpawnItem")
	{
		return new MapGenStep_SpawnItem(genStepXmlElement);
	}
	//TODO: Add CellularAutomata
	if(elementName == "CellularAutomata")
	{
		return new MapGenStep_CellularAutomata(genStepXmlElement);
	}
	//TODO: Add RoomsAndPaths
	if(elementName == "RoomsAndPaths")
	{

	}

	return nullptr;
}