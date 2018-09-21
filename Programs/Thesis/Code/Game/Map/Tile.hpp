#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Utility\Tags.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Definitions\TileDefinition.hpp"
#include "Engine\Core\Rgba.hpp"
#include <vector>


class Tile //:public Widget
{
public:

	Tile();
	Tile(const std::string& name)// : Widget(name)
	{
		//board creation
	}

	~Tile();
	void Initialize();

	void SetTileType(TileDefinition* newTileDefintion){ m_tileDefinition = newTileDefintion;}
	IntVector2 GetTileCoordinates(){ return m_tileCoords;}
	Vector2 GetWorldSpaceCoordinates();
	AABB2 GetWorldSpaceBounds();

	void Render();

public:
	IntVector2 m_tileCoords;
	AABB2 m_worldBounds;	//can optimize this later if memory size become more of a concern VS cpu dependency of calculating world bounds
	TileDefinition* m_tileDefinition = nullptr;
	Tags* m_tags = nullptr;
	Rgba m_tint = Rgba::WHITE;
};