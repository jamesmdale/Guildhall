#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\IntVector2.hpp"

class SpriteSheet
{
public:
	SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh);
	AABB2 GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const;
	AABB2 GetTexCoordsForSpriteIndex(const int& spriteIndex) const;
	int GetNumSprites() const;
	const Texture* GetSpriteSheetTexture() const;
	
private:
		const Texture& m_spriteSheetTexture;
		IntVector2 m_spriteLayout;
};