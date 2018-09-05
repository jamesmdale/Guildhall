#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Utility\Tags.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Definitions\TileDefinition.hpp"
#include <vector>


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
	Tags* m_tags = nullptr;;
};