#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "TileDefinition.hpp"
class Tile
{

public:
	void Update();
	void Render();
	//int GetTileIndex(); TODO: Make your life simpler

public:
	IntVector2 m_tileCoords;
	//int m_tileType;
	AABB2 m_tileBounds;
	TileDefinition* m_tileDefinition = nullptr;
};
