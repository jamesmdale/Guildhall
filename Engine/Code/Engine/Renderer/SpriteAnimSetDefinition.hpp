#pragma once
#include <map>
#include <string>
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class SpriteAnimDefinition;
class SpriteAnimSetDefinition
{
	friend class SpriteAnimSet;

public:
	SpriteAnimSetDefinition( const tinyxml2::XMLElement& animSetElement, Renderer& renderer );
	~SpriteAnimSetDefinition();

protected:
	std::map< std::string, SpriteAnimDefinition* >	m_namedAnimDefs;
	std::string	m_defaultAnimName = "Idle";
};
