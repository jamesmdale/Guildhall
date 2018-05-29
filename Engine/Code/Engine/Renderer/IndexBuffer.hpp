#pragma once
#include "Engine\Renderer\RenderBuffer.hpp"

class IndexBuffer : public RenderBuffer
{
public:
	IndexBuffer(){};
	IndexBuffer(int indexCount, int indexStride);
	~IndexBuffer();

public:
	int m_indexCount;
	int m_indexStride;
};


