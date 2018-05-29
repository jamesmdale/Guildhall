#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/PortalDefinition.hpp"

class AdventureMapPortalGenerationStep
{
public:
	explicit AdventureMapPortalGenerationStep( const tinyxml2::XMLElement& element );
public: 
	//list of tile definition member variables
	PortalDefinition* m_portalDefinition = nullptr;
	std::string m_onTileType = "default";
	std::string m_toMap = "default";
	std::string m_toTileType = "default";
	PortalDefinition* m_reciprocalType = nullptr;
};