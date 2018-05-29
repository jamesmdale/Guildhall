#include "Engine\Math\IntVector3.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <string>

const IntVector3 IntVector3::ZERO = IntVector3(0, 0, 0);
const IntVector3 IntVector3::ONE = IntVector3(1, 1, 1);
const IntVector3 IntVector3::RIGHT = IntVector3(1, 0, 0);
const IntVector3 IntVector3::UP = IntVector3(0, 1, 0);
const IntVector3 IntVector3::FORWARD = IntVector3(0, 0, 1);

//-----------------------------------------------------------------------------------------------
IntVector3::IntVector3( const IntVector3& copy )	
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

IntVector3::IntVector3(const IntVector2 & initialXY, int initialZ)
{
	x = initialXY.x;
	y = initialXY.y;
	z = initialZ;
}


//-----------------------------------------------------------------------------------------------
IntVector3::IntVector3( int initialX, int initialY, int initialZ )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator + ( const IntVector3& vecToAdd ) const
{
	return IntVector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator-( const IntVector3& vecToSubtract ) const
{
	return IntVector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator*( int uniformScale ) const
{
	return IntVector3( x * uniformScale, y * uniformScale, z * uniformScale); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator/( int inverseScale ) const
{
	return IntVector3( x/inverseScale, y/inverseScale, z/inverseScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator+=( const IntVector3& vecToAdd )
{
	x = x + vecToAdd.x;
	y = y + vecToAdd.y;
	z = z + vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator-=( const IntVector3& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
	z = z - vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator*=( const int uniformScale )
{
	x = x * uniformScale; 
	y = y * uniformScale; 
	z = z * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator/=( const int uniformDivisor )
{
	x = x / uniformDivisor;
	y = y / uniformDivisor;
	z = z / uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator=( const IntVector3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
bool IntVector3::operator==( const IntVector3& compare ) const
{
	bool isEqual = false;

	if(x == compare.x && y == compare.y && z == compare.z)
		isEqual = true;

	return isEqual;
}

//-----------------------------------------------------------------------------------------------
bool IntVector3::operator!=( const IntVector3& compare ) const
{
	bool isNotEqual = true;

	if(x == compare.x && y == compare.y && z == compare.z)
		isNotEqual = false;

	return isNotEqual;
}

IntVector3 operator*( int uniformScale, const IntVector3& vecToScale )
{
	return IntVector3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale); // #MP1Fixme
}


int GetDistanceSquared(const IntVector3& a, const IntVector3& b)
{
	int dx = a.x - b.x;
	int dy = a.y - b.y;
	int dz = a.z - b.z;

	return ((dx * dx) + (dy * dy) + (dz * dz));
}