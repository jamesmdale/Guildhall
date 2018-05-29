#pragma once
#include <string>
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Engine\Math\AABB2.hpp"

class BlockDefinition
{
public:
	BlockDefinition(){};

public:
	std::string m_name; 
	SpriteSheet* m_spriteSheet; 
	AABB2 m_uvTop; 
	AABB2 m_uvSide; 
	AABB2 m_uvBottom;
};