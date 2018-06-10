#pragma once
#include "Engine\Math\AABB2.hpp"
#include <string>
#include <map>
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\Texture.hpp"


class BitmapFont
{
	friend class Renderer;

public:
	AABB2 GetUVsForGlyph( int glyphUnicode ) const; // pass ‘A’ or 65 for A, etc.
	float GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale ) const;
	float GetGlyphAspect( int glyphUnicode ) const 
	{ 
		UNUSED(glyphUnicode); 
		return m_baseAspect;
	} // will change later (COMMENTED OUT TO AVOID WARNING)	

	const Texture* GetFontTexture();

private:
	explicit BitmapFont( const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect ); // private, can only called by Renderer (friend class)

public:
	const SpriteSheet&	m_spriteSheet; // used internally; assumed to be a 16x16 glyph sheet
	float			m_baseAspect = 1.0f; // used as the base aspect ratio for all glyphs
	std::string m_fontName;

};
