//TODO: update for functions and etc if necessary. Empty cpp for now
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"

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
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(Vector2((float)m_tileCoords.x, (float)m_tileCoords.y));
	g_theRenderer->Rotatef(0.f, 0.f, 0.f, 1.f);
	g_theRenderer->Scalef(1.f, 1.f, 0.f);

	g_theRenderer->DrawTexturedAABB(GetTileBounds(), *g_tileSpriteSheet->GetSpriteSheetTexture(),  m_tileDef->m_baseSpriteUVCoords.mins,  m_tileDef->m_baseSpriteUVCoords.maxs, m_tileDef->m_baseSpriteTint);	
	g_theRenderer->PopMatrix();

	if(m_tileDef->m_overlaySpriteCoords != IntVector2(-1,-1))
	{
		g_theRenderer->PushMatrix();
		g_theRenderer->Translatef(Vector2((float)m_tileCoords.x, (float)m_tileCoords.y));
		g_theRenderer->Rotatef(0.f, 0.f, 0.f, 1.f);
		g_theRenderer->Scalef(1.f, 1.f, 0.f);

		g_theRenderer->DrawTexturedAABB(GetTileBounds(), *g_tileSpriteSheet->GetSpriteSheetTexture(),  m_tileDef->m_overlaySpriteUVCoords.mins,  m_tileDef->m_overlaySpriteUVCoords.maxs, m_tileDef->m_overlaySpriteTint);	
		g_theRenderer->PopMatrix();
	}
}
//void Tile::Update()
//{
//
//}