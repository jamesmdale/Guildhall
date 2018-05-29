#include "Engine\Renderer\IndexBuffer.hpp"

IndexBuffer::IndexBuffer(int indexCount, int indexStride)
{
	m_indexCount = indexCount;
	m_indexStride = indexStride;
}

IndexBuffer::~IndexBuffer()
{
}
