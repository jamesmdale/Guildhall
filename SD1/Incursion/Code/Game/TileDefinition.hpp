#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"

enum SpriteTileType
{
	grass = 0,
	stone,
	water,
	barricade,
	sand,
	sandstone,
	start,
	finish,
	lava,
	slate,
	NUM_TILE_TYPES
};

class TileDefinition
{

public:
		TileDefinition();
		TileDefinition(SpriteTileType type, bool isSolid, IntVector2 spriteTileCoordinates, AABB2 textureCoords);
public:
	static SpriteSheet* s_spriteSheet;
	static TileDefinition s_tileDefinitions[NUM_TILE_TYPES];
	bool m_isSolid;
	SpriteTileType m_tileType;
	IntVector2 m_spriteTileCoordinates;
	AABB2 m_textureAABB2;
	static void Initialize();
	
};

