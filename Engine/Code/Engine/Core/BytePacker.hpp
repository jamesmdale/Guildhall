#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\EndianHelper.hpp"

#define BIT_FLAG(f) (1U << (f))

enum eBytePackerOptionBit : uint
{
	BYTEPACKER_DEFAULT = 0U,
	BYTEPACKER_OWNS_MEMORY	= BIT_FLAG(0),
	BYTEPACKER_CAN_GROW		= BIT_FLAG(1)
};

typedef uint eBytePackerOptions;

class BytePacker
{
public:
	//owns buffer. growable
	BytePacker(eEndianness byteOrder = LITTLE_ENDIAN);		

	//owns buffer. can't grow
	BytePacker(size_t bufferSize, eEndianness byteOrder = LITTLE_ENDIAN);	

	//doesn't own buffer, can't grow
	BytePacker(size_t bufferSize, void* buffer, eEndianness byteOrder = LITTLE_ENDIAN);
	~BytePacker();

	void SetEndianness(eEndianness endianness);
	bool SetReadableByteCount(size_t byteCount);

	bool WriteBytes(size_t byteCount, const void* data, bool doesConsiderEndianness = true);
	bool ReadBytes(void* outData, size_t maxByteCount, bool doesConsiderEndianness);

	size_t WriteSize(size_t size);
	size_t ReadSize(size_t* outsize);

	bool WriteString(const char* writeString);
	bool ReadString(char* outString, size_t maxByteSize);

	//read/write head helpers
	void ResetBuffer();
	void ResetHeads();
	void ResetWrite();
	void ResetRead();
	void MoveReadHead(size_t numBytes);

	uint16_t BytePacker::PeekBuffer(bool doesChangeToPlatformEndianness);

	eEndianness GetEndianness() const;
	size_t GetWrittenByteCount() const;
	size_t GetWriteableByteCount() const;
	size_t GetReadableByteCount() const;
	size_t GetBufferSize() const;

	void* GetBuffer() const;

	bool ExtendBufferSize(size_t newBufferSize);

private:
	eEndianness m_endianness = LITTLE_ENDIAN;
	size_t m_bufferSize = 0;

	size_t m_writtenByteCount = 0;
	size_t m_readByteCount = 0;

	eBytePackerOptionBit m_bytePackerOptions = BYTEPACKER_DEFAULT;
	void* m_buffer = nullptr;
};

