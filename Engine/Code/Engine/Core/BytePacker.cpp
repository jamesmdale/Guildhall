#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\BitHelper.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EndianHelper.hpp"

//  owns buffer, growable (Default - LITTLE_ENDIAN =============================================================================
BytePacker::BytePacker()
{
	m_endianness = LITTLE_ENDIAN;
	m_bufferSize = sizeof(uint8_t);
	m_buffer = malloc(m_bufferSize);

	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY);
	m_bytePackerOptions = (eBytePackerOptionBit)EnableBits<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_CAN_GROW);
}

//  owns buffer, growable =============================================================================
BytePacker::BytePacker(eEndianness byteOrder)
{
	m_endianness = byteOrder;
	m_bufferSize = sizeof(uint8_t);
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

//  deep copy constructor =============================================================================
BytePacker::BytePacker(const BytePacker& packer)
{
	m_bufferSize = packer.GetBufferSize();
	m_endianness = packer.GetEndianness();

	m_writtenByteCount = packer.GetWrittenByteCount();	
	m_writeHead = packer.GetWriteHead();
	m_readHead = packer.GetReadHead();

	m_buffer = malloc(m_bufferSize);

	memcpy(m_buffer, (void*)packer.GetBuffer(), m_bufferSize);	
}

//  =============================================================================
BytePacker::~BytePacker()
{
	if (AreBitsSet<uint>((uint)m_bytePackerOptions, (uint)BYTEPACKER_OWNS_MEMORY))
	{
		if (m_buffer != nullptr)
		{
			free(m_buffer);
			m_buffer = nullptr;
		}
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

//  =========================================================================================
void BytePacker::SetWriteHeadToMaxWritten()
{
	m_writeHead = m_writtenByteCount;
}

void BytePacker::SetWrittenByteCountToBufferSize()
{
	m_writtenByteCount = m_bufferSize;
}

//  =============================================================================
bool BytePacker::WriteBytes(size_t byteCount, const void* data, bool doesConsiderEndianness /* = true */)
{
	size_t numNewBytesToWrite = 0;
	if (m_writeHead != m_writtenByteCount)
	{
		if (m_writeHead + byteCount > m_writtenByteCount)
		{
			numNewBytesToWrite = (m_writeHead + byteCount) - m_writtenByteCount;
		}
	}
	else
	{
		numNewBytesToWrite = byteCount;
	}

	if (m_writtenByteCount + numNewBytesToWrite > m_bufferSize)
	{
		size_t newBufferSize = m_bufferSize * 2;
		//if greater than double, we will just go ahead and double the size according to the requested amount
		if (m_writtenByteCount + numNewBytesToWrite > m_bufferSize)
		{
			newBufferSize = m_writtenByteCount + numNewBytesToWrite * 2;
		}
		//we are beyond the buffersize and need to grow if possible (doubles)
		bool success = ExtendBufferSize(newBufferSize);

		//we weren't allowed to extend the buffer and therefore can't write
		if (!success)
			return false;
	}

	uint8_t val = *(uint8_t*)data;

	//write data
	memcpy((byte_t*)m_buffer + m_writeHead, data, byteCount);

	if(doesConsiderEndianness)
		ToEndianness(m_buffer, m_bufferSize, m_endianness);

	//update written byte count if we wrote to the end of the buffer
	m_writeHead += byteCount;
	m_writtenByteCount += numNewBytesToWrite;

	GUARANTEE_OR_DIE(m_writeHead <= m_writtenByteCount, "BYTEPACKER WRITE HEAD PAST WRITTEN AMOUNT");

	return true;
}

//  =============================================================================
bool BytePacker::ReadBytes(void* outData, size_t maxByteCount, bool doesConsiderEndianness)
{
	if(GetReadableByteCount() < maxByteCount)
		return false;

	memcpy(outData, (byte_t*)m_buffer + m_readHead, maxByteCount);

	if(doesConsiderEndianness)
		ToEndianness(m_buffer, m_bufferSize, m_endianness);

	m_readHead += maxByteCount;	

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
		memcpy(&nextRead, (byte_t*)m_buffer + m_readHead, 1);

		byte_t bitMaskCheck = nextRead & 0b1000'0000;
		if (bitMaskCheck != 0b1000'0000)
		{
			sizeReadComplete = true;
		}

		byte_t decodedByte = nextRead & 0b0111'1111;

		outSizeLocal = outSizeLocal | (decodedByte << (7 * readIteration));

		readIteration++;
		m_readHead++;
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

	bool success = ReadBytes(outString, readSize, false);

	return success;
}
//  =============================================================================
void BytePacker::ResetBuffer()
{
	delete(m_buffer);
	m_buffer = nullptr;

	ResetRead();
	ResetWrite();

	m_buffer = malloc(m_bufferSize);
}

//  =============================================================================
void BytePacker::ResetHeads()
{
	ResetWrite();
	ResetRead();
}

//  =============================================================================
void BytePacker::ResetWrite()
{
	m_writeHead = 0;
}

//  =============================================================================
void BytePacker::ResetRead()
{
	m_readHead = 0;
}

//  =============================================================================
void BytePacker::MoveWriteHead(size_t numBytes)
{
	size_t numByteCount = m_writeHead + numBytes;

	if (m_writeHead > m_writtenByteCount)
		m_writeHead = m_writtenByteCount;
	else if (numByteCount < 0)
		m_writeHead = 0;
	else
		m_writeHead = numByteCount;
}

//  =============================================================================
void BytePacker::MoveReadHead(size_t numBytes)
{
	size_t numByteCount = m_readHead + numBytes;

	if(numByteCount > m_writtenByteCount)
		m_readHead = m_writtenByteCount;
	else if(numByteCount < 0)
		m_readHead = 0;
	else
		m_readHead = numByteCount;
}

// =============================================================================
uint16_t BytePacker::PeekBuffer(bool doesChangeToPlatformEndianness)
{
	uint16_t packetSize = *(uint16_t*)m_buffer;

	if(doesChangeToPlatformEndianness)	
		FromEndianness((void*)&packetSize, 2, m_endianness);

	return packetSize;
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
	return m_writtenByteCount - m_readHead;
}

//  =============================================================================
size_t BytePacker::GetBufferSize() const
{
	return m_bufferSize;
}

//  =============================================================================
size_t BytePacker::GetWriteHead() const
{
	return m_writeHead;
}

//  =============================================================================
size_t BytePacker::GetReadHead() const
{
	return m_readHead;
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
