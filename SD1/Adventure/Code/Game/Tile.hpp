#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/TileExtraInfo.hpp"
class Tile
{
public:
	
	void SetTileType(TileDefinition* newTileDefintion){ m_tileDef = newTileDefintion;}
	IntVector2 GetTileCoordinates(){ return m_tileCoords;}
	AABB2 GetTileBounds();
	AABB2 GetWorldSpaceBounds();

	void Render();
	//void Update();
	
public:
	IntVector2 m_tileCoords;
	TileDefinition* m_tileDef = nullptr;
	TileExtraInfo* m_tileInfo = new TileExtraInfo();
};