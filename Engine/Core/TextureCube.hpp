#pragma once

class Image;

class TextureCube
{
public:
	TextureCube();
	~TextureCube();

	bool MakeFromImages(const Image* images);

	bool MakeFromImage(const Image& image);
	bool MakeFromImage(const char* fileName);

	inline int GetWidth() const { return m_size; }
	inline int GetHeight() const { return m_size; }

	inline bool IsValid() const { return (m_handle >= 0); }

	inline int GetHandle() const { return m_handle; }

public:
	int m_size;
	int m_handle = -1;
};

