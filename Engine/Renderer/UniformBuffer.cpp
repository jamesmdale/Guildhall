#include "Engine\Renderer\UniformBuffer.hpp"
#include <stdlib.h>

UniformBuffer::UniformBuffer(const size_t byte_count, const void* data)
{
	SetCPUData(byte_count, data);
	m_renderBuffer.buffer_size = byte_count;
}

UniformBuffer::~UniformBuffer()
{
	free(m_cpuBuffer);
	m_cpuBuffer = nullptr;
}

void UniformBuffer::SetCPUData(size_t byte_size, const void* data)
{
	/*if(m_cpuBuffer)
	{
		delete m_cpuBuffer;
		m_cpuBuffer = nullptr;
	}*/
	free(m_cpuBuffer);

	m_cpuBuffer = malloc(byte_size);
	memcpy(m_cpuBuffer, data, byte_size);

	m_isDirty = true;
}

void UniformBuffer::UpdateGPU()
{
	if(m_isDirty)
	{
		m_renderBuffer.CopyToGPU(m_renderBuffer.buffer_size, m_cpuBuffer);
		m_isDirty = false;
	}
}

void UniformBuffer::SetGPUData(size_t byte_size, const void* data)
{
	free(m_cpuBuffer);
	m_cpuBuffer = malloc(byte_size);
	memcpy(m_cpuBuffer, data, byte_size);

	m_renderBuffer.CopyToGPU(m_renderBuffer.buffer_size, m_cpuBuffer);

	m_isDirty = false;
}

void const* UniformBuffer::GetCPUBuffer() const
{
	return m_cpuBuffer;
}

void* UniformBuffer::GetCPUBuffer()
{
	m_isDirty = true;
	return m_cpuBuffer;	
}

size_t UniformBuffer::GetSize() const
{
	return m_renderBuffer.buffer_size;
}


