#include "Engine\Core\Image.hpp"
#include "Engine\ThirdParty\stb\stb_image.h"
#include "Engine\Core\EngineCommon.hpp"

Image::Image( const std::string& imageFilePath )
{
	m_ImagePath = imageFilePath;
	
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)
	unsigned char aData = 255;

	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested);	

	float numTexels = (float)m_dimensions.x * (float)m_dimensions.y;
	
	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		int offset = texelIndex * numComponents;

		if(numComponents == 4)
		{
			aData = imageData[offset + 3];
		}

		m_texels.push_back(Rgba(imageData[offset], imageData[offset + 1], imageData[offset + 2], aData));
	}
	
	stbi_image_free( imageData );
}

Image::Image( const IntVector2& dimensions, const Rgba& color, const std::string imageName )
{
	m_dimensions = IntVector2(dimensions.x, dimensions.y);
	m_ImagePath = imageName; //store the image name in the path

	int numTexels = dimensions.x * dimensions.y;
	for(int texelsIndex = 0; texelsIndex < numTexels; texelsIndex++)
	{
		m_texels.push_back(color);
	}
}

Rgba Image::GetTexel( int x, int y ) const 			// (0,0) is top-left
{	
	int texelIndex = y + (m_dimensions.x * x);

	return m_texels[texelIndex];
	
}

std::vector<Rgba> Image::GetTexels()
{
	return m_texels;
}

void* Image::GetTexelsAsByteArray()
{
	return m_texels.data();
}


const void* Image::GetTexelsAsData(int xOffset, int yOffset)
{
	//size_t offsetIndex = (m_dimensions.x * yOffset) + xOffset;
	int offsetIndex = (m_dimensions.x * yOffset) + xOffset;
	return &m_texels[offsetIndex];
}

void Image::SetTexel( int x, int y, const Rgba& color )
{
	int texelIndex = y + (m_dimensions.x * x);

	m_texels[texelIndex] = color;
}

