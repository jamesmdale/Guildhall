#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
#include <string>

class Image
{
public:
	explicit Image( const std::string& imageFilePath );
	explicit Image( const IntVector2& dimensions, const Rgba& color, const std::string imageNamePath );
	Rgba	GetTexel( int x, int y ) const; 			// (0,0) is top-left
	void	SetTexel( int x, int y, const Rgba& color );
	IntVector2 GetDimensions() const {return m_dimensions;}
	int GetWidth() const {return m_dimensions.x;}
	int GetHeight() const {return m_dimensions.y;}
	std::vector<Rgba> GetTexels();
	void* GetTexelsAsByteArray();
	const void* GetTexelsAsData(int xOffset, int yOffset);
	std::string GetImagePath(){return m_ImagePath;}

private:
	IntVector2		m_dimensions;
	std::vector< Rgba >	m_texels; // ordered left-to-right, then down, from [0]=(0,0) at top-left
	std::string m_ImagePath;
};