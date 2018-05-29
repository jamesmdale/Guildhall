#pragma once
#include "TileDefinition.hpp"
#include "GameCommon.hpp"

TileDefinition TileDefinition::s_tileDefinitions[NUM_TILE_TYPES];
SpriteSheet* TileDefinition::s_spriteSheet = nullptr;
TileDefinition::TileDefinition()
{
	
}


TileDefinition::TileDefinition(SpriteTileType type, bool isSolid, IntVector2 spriteTileCoordinates, AABB2 textureCoords)
{
	m_tileType = type;
	m_isSolid = isSolid;
	m_spriteTileCoordinates = spriteTileCoordinates;
	m_textureAABB2 = textureCoords;
}

void TileDefinition::Initialize()
{
	//Texture* terrain = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	TileDefinition::s_spriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), 8, 8);

	for(int tileDefIndex = 0; tileDefIndex < NUM_TILE_TYPES; tileDefIndex++)
	{
		bool isSolid = false;
		IntVector2 spriteCoords;
		switch (tileDefIndex)
		{
			case grass:
				isSolid = false;
				spriteCoords = IntVector2(0,0);
				break;
			case stone:
				isSolid = true;
				spriteCoords = IntVector2(7,3);
				break;
			case water:
				isSolid = false;
				spriteCoords = IntVector2(4,7);
				break;
			case barricade:
				isSolid = true;
				spriteCoords = IntVector2(5,6);
				break;
			case sand:
				isSolid = false;
				spriteCoords = IntVector2(7,1);
				break;
			case sandstone:
				isSolid = true;
				spriteCoords = IntVector2(7,2);
				break;
			case start:
				isSolid = false;
				spriteCoords = IntVector2(3,6);
				break;
			case finish:
				isSolid = true;
				spriteCoords = IntVector2(1,7);
				break;
			case lava:
				isSolid = false;
				spriteCoords = IntVector2(6,7);
				break;
			case slate:
				isSolid = false;
				spriteCoords = IntVector2(6,7);
				break;

				
		}		

		AABB2 textureCoords = s_spriteSheet->GetTexCoordsForSpriteCoords(spriteCoords);

		s_tileDefinitions[tileDefIndex] = TileDefinition((SpriteTileType)tileDefIndex, isSolid, spriteCoords, textureCoords);
	}
}