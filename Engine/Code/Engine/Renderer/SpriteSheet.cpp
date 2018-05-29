#include "Engine\Renderer\SpriteSheet.hpp"

SpriteSheet::SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh) : m_spriteSheetTexture(texture)
{
	m_spriteLayout = IntVector2(tilesWide, tilesHigh);
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const
{
	float minx = ((float)spriteCoords.x/(float)m_spriteLayout.x);
	float miny = ((float)spriteCoords.y/(float)m_spriteLayout.y);
	float maxx = ((float)(spriteCoords.x + 1)/(float)m_spriteLayout.x);
	float maxy = ((float)(spriteCoords.y + 1)/(float)m_spriteLayout.y);

	return AABB2(minx, 1.f - miny, maxx, 1.f - maxy); // have to flip coordinates because our image is upside down
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(const int& spriteIndex) const
{
	IntVector2 spriteCoords;

	spriteCoords.x = spriteIndex % m_spriteLayout.x;
	spriteCoords.y = spriteIndex / m_spriteLayout.x;
	
	return GetTexCoordsForSpriteCoords(spriteCoords);
}

int SpriteSheet::GetNumSprites() const
{
	return m_spriteLayout.x * m_spriteLayout.y;
}

const Texture* SpriteSheet::GetSpriteSheetTexture() const
{
	return &m_spriteSheetTexture;
}

