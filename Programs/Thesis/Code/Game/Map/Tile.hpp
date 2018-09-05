#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Utility\Tags.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Definitions\TileDefinition.hpp"
#include <vector>


class Tile : public Widget
{
public:

	Tile();
	Tile(const std::string& name) : Widget(name)
	{
		//board creation
	}

	virtual ~Tile() override;
	virtual void Initialize() override;

	void SetTileType(TileDefinition* newTileDefintion){ m_tileDefinition = newTileDefintion;}
	IntVector2 GetTileCoordinates(){ return m_tileCoords;}
	AABB2 GetTileBounds();
	AABB2 GetWorldSpaceBounds();

	void Render();
	//void Update();
public:
	IntVector2 m_tileCoords;
	TileDefinition* m_tileDefinition = nullptr;
	Tags* m_tags = nullptr;;
};