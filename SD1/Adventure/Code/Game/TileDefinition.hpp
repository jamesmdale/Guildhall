#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class TileDefinition
{
public:
	explicit TileDefinition( const tinyxml2::XMLElement& element );
	static void Initialize(const std::string& filePath);
public: 
	//list of tile definition member variables
	std::string m_Name = "default";
	IntVector2 m_baseSpriteCoords = IntVector2(0,0);
	AABB2 m_baseSpriteUVCoords = AABB2(Vector2(0.f, 0.f), Vector2(0.f,0.f));
	Rgba m_baseSpriteTint = Rgba(1.f, 1.f, 1.f, 1.f);

	IntVector2 m_overlaySpriteCoords = IntVector2(-1,-1); //invalid
	AABB2 m_overlaySpriteUVCoords = AABB2(Vector2(0.f, 0.f), Vector2(0.f,0.f));
	Rgba m_overlaySpriteTint = Rgba(1.f, 1.f, 1.f, 1.f);

	bool m_allowSight = false;
	bool m_allowsWalking = false;
	bool m_allowsFlying = false;
	bool m_allowsSwimming = false;

	//static variables
	static SpriteSheet* s_tileSpriteSheet;
	static std::map< std::string, TileDefinition* >	s_definitions;
};
