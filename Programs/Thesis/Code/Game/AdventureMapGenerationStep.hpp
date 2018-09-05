#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/AdventureMapPortalGenerationStep.hpp"

class AdventureMapGenerationStep
{
public:
	explicit AdventureMapGenerationStep( const tinyxml2::XMLElement& element );
public: 
	//list of tile definition member variables
	std::string m_mapName = "default";
	MapDefinition* m_mapDefinition = nullptr;
	
	std::map<std::string, AdventureMapPortalGenerationStep* > m_mapPortals;
	//std::map<std::string, AdventureMapActorGenerationStep* > m_mapActors;
	//std::map<std::string, AdventureMapItemGenerationStep* > m_mapItems;


	//static variables
	static std::map< std::string, AdventureMapGenerationStep* >	s_mapGenerationSteps;
};