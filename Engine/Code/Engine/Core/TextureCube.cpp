#include "Engine\Core\TextureCube.hpp"
#include "Engine\Core\Image.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Core\EngineCommon.hpp"

TextureCube::TextureCube()
{
	m_size = 0;
	m_handle = 0;
}

TextureCube::~TextureCube()
{
	if (IsValid()) {
		glDeleteTextures( 1, (GLuint*)&m_handle );
		m_handle = NULL; 
	}

	m_size = 0;
}

bool TextureCube::MakeFromImage(Image& image)
{
	int imageWidth = image.GetWidth(); 
	int size = imageWidth / 4;

	ASSERT_OR_DIE(image.GetHeight() == size * 3, "TEXTURE CUBE IMAGE INCORRECT FORMAT");
	ASSERT_OR_DIE(image.GetHeight() == image.GetHeight(), "TEXTURE CUBE IMAGE INCORRECT FORMAT: HEIGHTS NOT EQUAL");

	if (m_handle == NULL) {
		glGenTextures( 1, (GLuint*)&m_handle ); 
		ASSERT_OR_DIE(IsValid(), "TEXTURE CUBE TEXTURE GENERATION FAILED");
	}

	m_size = size;

	//hard set formats as our engine only supports one at the moment
	TODO("Add more format support");
	GLenum internalFormat = GL_RGBA8;
	GLenum channels = GL_RGBA;
	GLenum pixelLayout = GL_UNSIGNED_BYTE;

	// bind it; 
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle ); 
	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, internalFormat, m_size, m_size ); 
	GL_CHECK_ERROR(); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, image.GetWidth() ); 
	GL_CHECK_ERROR(); 

	// bind the image to the side; 
	FlipAndBindImage( TEXCUBE_RIGHT,  image, channels, pixelLayout ); 
	FlipAndBindImage( TEXCUBE_LEFT,   image, channels, pixelLayout ); 
	FlipAndBindImage( TEXCUBE_TOP,    image, channels, pixelLayout ); 
	FlipAndBindImage( TEXCUBE_BOTTOM, image, channels, pixelLayout ); 
	FlipAndBindImage( TEXCUBE_FRONT,  image, channels, pixelLayout ); 
	FlipAndBindImage( TEXCUBE_BACK,   image, channels, pixelLayout ); 

	return GLSucceeded(); 
}

bool TextureCube::MakeFromImage(const char* fileName)
{
	Image image = Image(fileName);

	return MakeFromImage(image);	
}


static void BindImageToSide( eTextureCubeSide side, Image& image, int size, int offsetX, int offsetY, GLenum channels, GLenum pixel_layout ) 
{
	const void* ptr = image.GetTexelsAsData(offsetX, offsetY);
	glTexSubImage2D( GetGLCubeSide(side),
		0,          // mip_level
		0, 0,       // offset
		size, size, 
		channels, 
		pixel_layout, 
		ptr ); 

	GL_CHECK_ERROR(); 
}

static void FlipAndBindImage( eTextureCubeSide side, Image& image, GLenum channels, GLenum pixelLayout ) 
{ 
	BindImageToSide( side, image, image.GetWidth(), 0, 0, channels, pixelLayout ); 
}

void TextureCube::Cleanup()
{
	if (IsValid())
	{
		glDeleteTextures(1, (GLuint*)&m_handle);
		m_handle = NULL;
	}

	m_size = 0;
}

static GLenum GetGLCubeSide(eTextureCubeSide side)
{
	GLenum convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	switch (side)
	{
	case TEXCUBE_RIGHT:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	case TEXCUBE_LEFT:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	case TEXCUBE_TOP:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	case TEXCUBE_BOTTOM:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	case TEXCUBE_FRONT:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	case TEXCUBE_BACK:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	}

	return convertedTex;
}


