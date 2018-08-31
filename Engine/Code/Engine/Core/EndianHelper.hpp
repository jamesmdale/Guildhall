#pragma once
#include "Engine\Core\EngineCommon.hpp"

enum eEndianness
{
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN
};

constexpr eEndianness GetPlatFormEndianness()
{
	const uint num = 1;
	const byte_t* array = (const byte_t*)&num;

	if (array[0] == 0x01)
		return LITTLE_ENDIAN;
	
	return BIG_ENDIAN;
}

// Assumes data is in platform endianness, and will convert to supplied endianness; 
void ToEndianness(void* outBuffer, size_t bufferSize, eEndianness endianness)
{
	if (GetPlatFormEndianness() == endianness)
		return; //values match. leave buffer alone

	byte_t* byteBuffer = (byte_t*)outBuffer;
	uint i = 0;
	uint j = bufferSize - 1U;

	while (i < j)
	{
		std::swap(byteBuffer[j], byteBuffer[i]);
		++i;
		--j;
	}
}

// Assumes data is in supplied endianness, and will convert to platform's endianness
void FromEndianness(void* outBuffer, size_t bufferSize, eEndianness endianness)
{
	if (GetPlatFormEndianness() == endianness)
		return; //values match. leave buffer alone

	byte_t* byteBuffer = (byte_t*)outBuffer;
	uint i = 0;
	uint j = bufferSize - 1U;

	while (i < j)
	{
		std::swap(byteBuffer[j], byteBuffer[i]);
		++i;
		--j;
	}
}
