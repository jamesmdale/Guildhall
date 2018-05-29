#include "Engine\Renderer\VertexBuffer.hpp"

VertexBuffer::VertexBuffer(int vertexCount, int vertexStride)
{
	m_vertexCount = vertexCount;
	m_vertexStride = vertexStride;
}

VertexBuffer::~VertexBuffer()
{
}
