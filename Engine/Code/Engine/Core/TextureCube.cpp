#include "Engine\Core\TextureCube.hpp"
#include "Engine\Core\Image.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"

TextureCube::TextureCube()
{
	m_size = 0;
	m_handle = 0;
}

TextureCube::~TextureCube()
{
	if (IsValid()) {
		glDeleteTextures( 1, (GLuint*)&m_handle );
		GL_CHECK_ERROR();

		m_handle = NULL; 
	}

	m_size = 0;
}

GLenum GetGLCubeSide(eTextureCubeSide side)
{
	GLenum convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	switch (side)
	{
	case TEXCUBE_RIGHT:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
	case TEXCUBE_LEFT:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		break;
	case TEXCUBE_TOP:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		break;
	case TEXCUBE_BOTTOM:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		break;
	case TEXCUBE_FRONT:
		convertedTex = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		break;
	case TEXCUBE_BACK:
		convertedTex = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		break;
	}

	return convertedTex;
}

void BindImageToSide(eTextureCubeSide side, Image& image, int size, int offsetX, int offsetY, GLenum channels, GLenum pixelLayout)
{
	const void* ptr = image.GetTexelsAsData(offsetX, offsetY);
	glTexSubImage2D(GetGLCubeSide(side),
		0,          // mip_level
		0, 0,       // offset
		size, size,
		channels,
		pixelLayout,
		ptr);

	GL_CHECK_ERROR();
}

void TextureCube::Cleanup()
{
	if (IsValid())
	{
		glDeleteTextures(1, (GLuint*)&m_handle);
		GL_CHECK_ERROR();
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
		GL_CHECK_ERROR();
		ASSERT_OR_DIE(IsValid(), "TEXTURE CUBE TEXTURE GENERATION FAILED");
	}

	m_size = size;

	//hard set formats as our engine only supports one at the moment
	TODO("Add more format support & ability to pass in other formats");
	GLenum internalFormat = GL_RGBA8;
	GLenum channels = GL_RGBA;
	GLenum pixelLayout = GL_UNSIGNED_BYTE;

	// bind it; 
	//glActiveTexture(GL_TEXTURE0 + 8);   test for bug

	glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle ); 
	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, internalFormat, m_size, m_size ); 
	GL_CHECK_ERROR(); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, image.GetWidth() ); 
	GL_CHECK_ERROR(); 

	// bind the image to the side; 
	BindImageToSide( TEXCUBE_RIGHT,  image, m_size, m_size * 2, m_size * 1, channels, pixelLayout ); 
	BindImageToSide( TEXCUBE_LEFT,   image, m_size, m_size * 0, m_size * 1, channels, pixelLayout ); 
	BindImageToSide( TEXCUBE_TOP,    image, m_size, m_size * 1, m_size * 0, channels, pixelLayout ); 
	BindImageToSide( TEXCUBE_BOTTOM, image, m_size, m_size * 1, m_size * 2, channels, pixelLayout ); 
	BindImageToSide( TEXCUBE_FRONT,  image, m_size, m_size * 1, m_size * 1, channels, pixelLayout ); 
	BindImageToSide( TEXCUBE_BACK,   image, m_size, m_size * 3, m_size * 1, channels, pixelLayout ); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ); 
	GL_CHECK_ERROR(); 

	return GLSucceeded(); 
}

bool TextureCube::MakeFromImage(const char* fileName)
{
	Image image = *Renderer::GetInstance()->CreateOrGetImage(fileName);

	return MakeFromImage(image);	
}



