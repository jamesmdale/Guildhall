#pragma once
#include "Engine\Renderer\RenderBuffer.hpp"

class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer(){};
	VertexBuffer(int vertexCount, int vertexStride);
	~VertexBuffer();
public:
	int m_vertexCount;
	int m_vertexStride;
};

