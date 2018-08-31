#include "Engine\Core\BytePacker.hpp"


//  =============================================================================
BytePacker::BytePacker(eEndianness byteOrder)
{

}

//  =============================================================================
BytePacker::BytePacker(size_t bufferSize, eEndianness byteOrder)
{
}

//  =============================================================================
BytePacker::BytePacker(size_t bufferSize, void * buffer, eEndianness)
{
}

//  =============================================================================
BytePacker::~BytePacker()
{
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
bool BytePacker::WriteBytes(size_t byteCount, const void * data)
{
	return false;
}

//  =============================================================================
size_t BytePacker::ReadBytes(void * outData, size_t maxByteCount)
{
	return size_t();
}

//  =============================================================================
size_t BytePacker::WriteSize(size_t size)
{
	return size_t();
}

//  =============================================================================
size_t BytePacker::ReadSize(size_t * outsize)
{
	return size_t();
}

//  =============================================================================
bool BytePacker::WriteString(const char * writeString)
{
	return false;
}

//  =============================================================================
bool BytePacker::ReadString(char * outString, size_t maxByteSize)
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
	return eEndianness();
}

//  =============================================================================
size_t BytePacker::GetWrittenByteCount() const
{
	return size_t();
}
//  =============================================================================
size_t BytePacker::GetWriteableByteCount() const
{
	return size_t();
}
//  =============================================================================
size_t BytePacker::GetReadableByteCount() const
{
	return size_t();
}
