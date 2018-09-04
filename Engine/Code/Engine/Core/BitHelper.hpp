#pragma once

//helpful and common bitwise operations

//enable the bits in 'storage' defined in 'setbits'
template<typename T>
T EnableBits(const T& storage, const T& setBits)
{
	return storage | setBits;
};

//disable the bits in 'storage' defined in 'setbits'
template<typename T>
T DisableBits(const T& storage, const T& setBits)
{
	T flippedSetBits = ~setBits;
	return storage & flippedSetBits;
};

//ex: powerOfTwo=1 == value/2; powerofTwo=2 == value/4
template< typename T>
T BitwiseDivide(const T& value, const int powerOfTwo)
{
	return value >> powerOfTwo;
}

//ex: powerOfTwo=1 == value*2; powerofTwo=2 == value*4
template<typename T>
T BitwiseMultiply(const T& value, const int powerOfTwo)
{
	return value << powerOfTwo;
}

template<typename T> 
bool AreBitsSet(const T& storage, const T& checkBits)
{
	T bits = storage & checkBits;

	if (bits == checkBits)
		return true;

	return false;
}
