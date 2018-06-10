#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine\ThirdParty\stb\stb_image.h"


BitmapFont::BitmapFont( const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect ) : m_spriteSheet(glyphSheet), m_baseAspect(baseAspect)  // private, can only called by Renderer (friend class)
{	
	m_fontName = fontName;	
}

AABB2 BitmapFont::GetUVsForGlyph( int glyphUnicode ) const // pass ‘A’ or 65 for A, etc.
{
	AABB2 glyph = m_spriteSheet.GetTexCoordsForSpriteIndex(glyphUnicode);
	return glyph;
}

float BitmapFont::GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale ) const
{
	return (asciiText.length() * (cellHeight * (aspectScale * m_baseAspect)));	 
}

const Texture* BitmapFont::GetFontTexture()
{
	return m_spriteSheet.GetSpriteSheetTexture();
}


