//TODO: update for functions and etc if necessary. Empty cpp for now
#include "Game\Map\Tile.hpp"
#include "Game\GameCommon.hpp"

AABB2 Tile::GetTileBounds()
{	
	return AABB2(Vector2(0, 0), Vector2(1, 1));
}

AABB2 Tile::GetWorldSpaceBounds()
{
	float xCoordinate = (float)m_tileCoords.x;
	float yCoordinate = (float)m_tileCoords.y;
	return AABB2(Vector2((float)xCoordinate, (float)yCoordinate),Vector2((float)xCoordinate + 1.f, (float)yCoordinate + 1.f)); //tiles are 1 across
}

void Tile::Render()
{

}
//void Tile::Update()
//{
//
//}