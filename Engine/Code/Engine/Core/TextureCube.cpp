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

bool TextureCube::MakeFromImages(const Image* images)
{
	if(IsValid())
	{
		glGenTextures(1, (GLuint*)&m_handle);
	}

	m_size = images[0].GetWidth();

	return true;
}

bool TextureCube::MakeFromImage(const Image& image)
{
	UNUSED(image);
	return false;
}

bool TextureCube::MakeFromImage(const char* fileName)
{
	UNUSED(fileName);
	return false;
}
