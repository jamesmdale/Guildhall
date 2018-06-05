#pragma once

enum eTextureCubeSide
{
	TEXCUBE_RIGHT,
	TEXCUBE_LEFT,
	TEXCUBE_TOP,
	TEXCUBE_BOTTOM,
	TEXCUBE_FRONT,
	TEXCUBE_BACK,
	NUM_TEXCUBE_SIDES
};

class Image;

class TextureCube
{
public:
	TextureCube();
	~TextureCube();

	bool MakeFromImage(Image& image);
	bool MakeFromImage(const char* fileName);

	int GetWidth() const { return m_size; }
	int GetHeight() const { return m_size; }

	bool IsValid() const { return (m_handle >= 0); }

	int GetHandle() const { return m_handle; }

	void Cleanup();

public:
	int m_size;
	int m_handle = -1;
};
