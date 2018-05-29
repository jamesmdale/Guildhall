#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include <string>
#include "Game/AdventureMapGenerationStep.hpp"

class AdventureDefinition
{
public:
	explicit AdventureDefinition( const tinyxml2::XMLElement& element );
	static void Initialize(const std::string& filePath);
public: 
	//list of tile definition member variables
	std::string m_name = "default";
	std::string m_title = "default";
	std::string m_startMapName = "default";
	std::string m_startOnTileType = "default";

	//victory conditions
	std::string m_haveDied = "default";
	//std::string m_haveCollected = "default";
	//std::string m_haveReturnedItemToActorName = "default";

	std::map<std::string, AdventureMapGenerationStep*> m_mapsToGenerate;
	//static variables
	static std::map< std::string, AdventureDefinition* >	s_definitions;
};