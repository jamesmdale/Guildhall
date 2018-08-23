//-----------------------------------------------------------------------------------------------
// Texture.cpp
//
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\ThirdParty\stb\stb_image.h"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Core\Image.hpp"

Texture::Texture(): m_textureID( 0 )
, m_dimensions( 0, 0 )
{

}

//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
Texture::Texture( const std::string& imageFilePath)
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

	GL_CHECK_ERROR();
	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	stbi_set_flip_vertically_on_load(1);
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	stbi_set_flip_vertically_on_load(0);
	PopulateFromData( imageData, m_dimensions, numComponents );
	stbi_image_free( imageData );
}

Texture::Texture( Image& image )
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = (unsigned char*)image.GetTexelsAsByteArray();
	PopulateFromData( imageData, image.GetDimensions(), numComponents );	
}


//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{
	m_dimensions = texelSize;

	int mipCount = CalculateMipCount(m_dimensions);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	GL_CHECK_ERROR();

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );
	GL_CHECK_ERROR();

	glActiveTexture(GL_TEXTURE0);
	GL_CHECK_ERROR();

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );
	GL_CHECK_ERROR();

	GLenum internalFormat = GL_RGBA8;	
	GLenum channels = GL_RGBA;
	GLenum pixelLayout = GL_UNSIGNED_BYTE;

	//if jpg
	if( numComponents == 3 )
	{
		channels = GL_RGB;
		internalFormat = GL_RGB8;
	}

	glTexStorage2D(GL_TEXTURE_2D, mipCount, internalFormat, m_dimensions.x, m_dimensions.y);

	GL_CHECK_ERROR();

	glTexSubImage2D(GL_TEXTURE_2D,
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		0, 0,				//offset of image (since we only have one they are zero
		m_dimensions.x,		// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		m_dimensions.y,		// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		channels,			// r-g-b-a
		pixelLayout,		//data type
		imageData);

	glGenerateMipmap(GL_TEXTURE_2D);

	GL_CHECK_ERROR();
}

bool Texture::CreateRenderTarget(int width, int height, TextureFormatType format)
{
	// generate the link to this texture
	glGenTextures( 1, &m_textureID ); 
	if (m_textureID == NULL) {
		return false; 
	}

	TODO("add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat ) when more texture formats are required");
	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE;  
	if (format == TEXTURE_FORMAT_D24S8) {
		internal_format = GL_DEPTH_STENCIL; 
		channels = GL_DEPTH_STENCIL; 
		pixel_layout = GL_UNSIGNED_INT_24_8; //Forseth had GL_UNSIGNED_INT_D24_S8? ?
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, m_textureID );    // bind our texture to our current texture unit (0)
	GL_CHECK_ERROR();

												 // Copy data into it;
	glTexImage2D( GL_TEXTURE_2D, 0, 
		internal_format, // what's the format OpenGL should use
		width, 
		height,        
		0,             // border, use 0
		channels,      // how many channels are there?
		pixel_layout,  // how is the data laid out
		nullptr );     // don't need to pass it initialization data 

	GL_CHECK_ERROR();	// make sure it suceeded

	

	// cleanup after myself; 
	glBindTexture( GL_TEXTURE_2D, NULL ); // unset it; 
	GL_CHECK_ERROR();

										  // Save this all off
	m_dimensions.x = width;  
	m_dimensions.y = height; 


	TODO("Save format asa member variable on texture as necessary");
	//m_format = fmt; // I save the format with the texture
					// for sanity checking.
	GL_CHECK_ERROR();
	return true; 
}

unsigned int Texture::GetHandle()
{
	return m_textureID;
}

Texture * Texture::CreateCompatible(const Texture& texture)
{
	Texture* outTexture = new Texture();
	outTexture->CreateRenderTarget(texture.m_dimensions.x, texture.m_dimensions.y, TEXTURE_FORMAT_RGBA8);

	return outTexture;
}

IntVector2 Texture::GetDimensions()
{
	return m_dimensions;
}

int Texture::CalculateMipCount(const IntVector2& dimensions)
{
	int value = GetLargerOfXY(dimensions);

	float mipCount = log2f((float)value);
	
	//round count up to make sure it's larger than the max dimension
	mipCount++;
	return (int)mipCount; //takes the floor of the calculated mipcount to make it a whole number
}