#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\BitHelper.hpp"


//  =============================================================================
BytePacker::BytePacker(eEndianness byteOrder)
{
	m_endianness = byteOrder;
	m_bufferSize = sizeof(uint);
	m_buffer = malloc(m_bufferSize);

	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY);
	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_CAN_GROW);
}

//  =============================================================================
BytePacker::BytePacker(size_t bufferSize, eEndianness byteOrder)
{
	m_bufferSize = m_bufferSize;
	m_endianness = byteOrder;

	m_buffer = malloc(bufferSize);

	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY);
}

//  =============================================================================
BytePacker::BytePacker(size_t bufferSize, void* buffer, eEndianness byteOrder)
{
	m_bufferSize = m_bufferSize;
	m_endianness = byteOrder;
	m_buffer = buffer;

	m_bytePackerOptions = BYTEPACKER_DEFAULT;
}

//  =============================================================================
BytePacker::~BytePacker()
{
	if (AreBitsSet<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY))
	{
		free(m_buffer);
		m_buffer = nullptr;
	}
}

//  =============================================================================
void BytePacker::SetEndianness(eEndianness endianness)
{
	m_endianness = endianness;
}

//  =============================================================================
bool BytePacker::SetReadableByteCount(size_t byteCount)
{
	return false;
}

//  =============================================================================
bool BytePacker::WriteBytes(size_t byteCount, const void* data)
{
	return false;
}

//  =============================================================================
size_t BytePacker::ReadBytes(void* outData, size_t maxByteCount)
{
	return size_t();
}

//  =============================================================================
size_t BytePacker::WriteSize(size_t size)
{
	return size_t();
}

//  =============================================================================
size_t BytePacker::ReadSize(size_t* outsize)
{
	return size_t();
}

//  =============================================================================
bool BytePacker::WriteString(const char* writeString)
{
	return false;
}

//  =============================================================================
bool BytePacker::ReadString(char* outString, size_t maxByteSize)
{
	return false;
}

//  =============================================================================
void BytePacker::ResetWrite()
{
}

//  =============================================================================
void BytePacker::ResetRead()
{
}

//  =============================================================================
eEndianness BytePacker::GetEndianness() const
{
	return m_endianness;
}

//  =============================================================================
size_t BytePacker::GetWrittenByteCount() const
{
	return m_writtenByteCount;
}
//  =============================================================================
size_t BytePacker::GetWriteableByteCount() const
{
	return m_bufferSize - m_writtenByteCount;
}
//  =============================================================================
size_t BytePacker::GetReadableByteCount() const
{
	return m_bufferSize;
}
