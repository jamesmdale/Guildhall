//-----------------------------------------------------------------------------------------------
// Texture.hpp
//
#pragma once
#include "Engine\Math\IntVector2.hpp"
#include <string>
#include <map>
#include "Engine\Core\Image.hpp"


enum TextureFormatType
{
	TEXTURE_FORMAT_RGBA8, //default color format
	TEXTURE_FORMAT_D24S8,
	NUM_TEXTURE_FORMAT_TYPES
};

//---------------------------------------------------------------------------
class Texture
{
	friend class Renderer; // Textures are managed by a Renderer instance
	friend class FrameBuffer;
	friend class ForwardRenderingPath;
	friend class ForwardRenderingPath2D;

public:
	IntVector2 GetDimensions();
	int CalculateMipCount(const IntVector2& dimensions);
private:
	Texture();
	//Texture(const Texture* copyTexture);
	Texture( const std::string& imageFilePath ); // Use renderer->CreateOrGetTexture() instead!
	Texture ( Image& image );

	void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents );
	bool CreateRenderTarget(int width, int height, TextureFormatType format);
	unsigned int GetHandle();
	static Texture* CreateCompatible(const Texture& texture);
	
private:
	unsigned int								m_textureID;
	IntVector2									m_dimensions;	
};



