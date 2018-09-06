#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\EndianHelper.hpp"

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
