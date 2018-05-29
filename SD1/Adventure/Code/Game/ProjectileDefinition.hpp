#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/EntityDefinition.hpp"
#include "engine/math/IntRange.hpp"

class ProjectileDefinition : EntityDefinition
{
public:
	explicit ProjectileDefinition( const tinyxml2::XMLElement& element, Renderer& renderer );
	static void Initialize(const std::string& filePath, Renderer& renderer);
public: 
	IntRange m_damageAmount = IntRange(0,1);

	static std::map< std::string, ProjectileDefinition* >	s_projectileDefinitions;
};