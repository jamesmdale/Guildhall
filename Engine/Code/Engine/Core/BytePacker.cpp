#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\BitHelper.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EndianHelper.hpp"


//  owns buffer, growable =============================================================================
BytePacker::BytePacker(eEndianness byteOrder)
{
	m_endianness = byteOrder;
	m_bufferSize = sizeof(uint);
	m_buffer = malloc(m_bufferSize);

	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY);
	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_CAN_GROW);
}

//  owns buffer, cant grow =============================================================================
BytePacker::BytePacker(size_t bufferSize, eEndianness byteOrder)
{
	m_bufferSize = m_bufferSize;
	m_endianness = byteOrder;

	m_buffer = malloc(bufferSize);

	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY);
}

//  doesn't own buffer can't grow =============================================================================
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
	m_writtenByteCount = byteCount;
	return true;
}

//  =============================================================================
bool BytePacker::WriteBytes(size_t byteCount, const void* data, bool doesConsiderEndianness /* = true */)
{
	if(m_writtenByteCount + byteCount > m_bufferSize)
	{
		size_t newBufferSize = m_bufferSize * 2;
		//if greater than double, we will just go ahead and double the size according to the requested amount
		if (m_writtenByteCount + byteCount > m_bufferSize)
		{
			newBufferSize =  m_writtenByteCount + byteCount * 2;
		}
		//we are beyond the buffersize and need to grow if possible (doubles)
		bool success = ExtendBufferSize(newBufferSize);

		//we weren't allowed to extend the buffer and therefore can't write
		if(!success)
			return false;
	}

	memcpy((byte_t*)m_buffer + m_writtenByteCount, data, byteCount);

	if(doesConsiderEndianness)
		ToEndianness(m_buffer, m_bufferSize, m_endianness);

	//update written byte count
	m_writtenByteCount += byteCount;

	return true;
}

//  =============================================================================
bool BytePacker::ReadBytes(void* outData, size_t maxByteCount)
{
	if(GetReadableByteCount() < maxByteCount)
		return false;

	memcpy(outData, (byte_t*)m_buffer + m_readByteCount, maxByteCount);

	ToEndianness(m_buffer, m_bufferSize, m_endianness);

	m_readByteCount += maxByteCount;	

	return true;
}

//  =============================================================================
size_t BytePacker::WriteSize(size_t size)
{
	size_t amountBytesWritten = 0;
	while (size != 0)
	{
		size_t value = size & 0b0111'1111;

		size = size >> 7;

		if(size != 0)
			value = value | 0b1000'0000;

		WriteBytes(1, &value, false);
		amountBytesWritten++;
	}

	return amountBytesWritten;
}

//  =============================================================================
size_t BytePacker::ReadSize(size_t* outsize)
{
	size_t readIteration = 0;
	size_t outSizeLocal = 0;
	size_t nextRead = 0;

	bool sizeReadComplete = false;

	while (!sizeReadComplete)
	{
		nextRead = 0;
		memcpy(&nextRead, (byte_t*)m_buffer + m_readByteCount, 1);

		byte_t bitMaskCheck = nextRead & 0b1000'0000;
		if (bitMaskCheck != 0b1000'0000)
		{
			sizeReadComplete = true;
		}

		byte_t decodedByte = nextRead & 0b0111'1111;

		outSizeLocal = outSizeLocal | (decodedByte << (7 * readIteration));

		readIteration++;
		m_readByteCount++;
	}

	*outsize = outSizeLocal;

	return readIteration;
}

//  =============================================================================
bool BytePacker::WriteString(const char* writeString)
{
	int character = 0;
	std::string convertedString (writeString);
	
	//determine the encoded size
	size_t writtenSize = WriteSize(convertedString.size() + 1);

	//write the data to the buffer	
	bool success = WriteBytes(convertedString.size() + 1, writeString, false);

	return success;
}

//  =============================================================================
bool BytePacker::ReadString(char* outString, size_t maxByteSize)
{
	//uses read size
	size_t readSize = 0;
	size_t readBytes = ReadSize(&readSize);

	if (readBytes > maxByteSize)
	{
		readBytes = maxByteSize;
	}

	bool success = ReadBytes(outString, readSize);

	return success;
}

//  =============================================================================
void BytePacker::ResetWrite()
{
	//must move readbytecount to be less than written bytecount
	m_writtenByteCount = 0;
	m_readByteCount = 0;
}

//  =============================================================================
void BytePacker::ResetRead()
{
	m_readByteCount = 0;
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
	if(AreBitsSet<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_CAN_GROW))
		return (size_t)GetInfinity();
	
	return m_bufferSize - m_writtenByteCount;
}

//  =============================================================================
size_t BytePacker::GetReadableByteCount() const
{
	return m_writtenByteCount - m_readByteCount;
}

//  =============================================================================
void* BytePacker::GetBuffer() const
{
	return m_buffer;
}

//  =============================================================================
bool BytePacker::ExtendBufferSize(size_t newBufferSize)
{
	//make sure we have the permissions to grow the value then double the size of the buffer
	if (AreBitsSet<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_CAN_GROW))
	{
		void* newBuffer = malloc(newBufferSize);

		memcpy(newBuffer, m_buffer, m_bufferSize);

		free(m_buffer);
		m_buffer = nullptr;

		m_buffer = newBuffer;
		m_bufferSize = newBufferSize;

		return true;
	}
	
	//we don't have the rights
	return false;
}
