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

	inline int GetWidth() const { return m_size; }
	inline int GetHeight() const { return m_size; }

	inline bool IsValid() const { return (m_handle >= 0); }

	inline int GetHandle() const { return m_handle; }

	void Cleanup();

public:
	int m_size;
	int m_handle = -1;
};
