#include "Engine\Math\IntVector2.hpp"
#include <string>
#include "Engine\Math\Vector2.hpp"

const IntVector2 IntVector2::UP = IntVector2(0, 1);
const IntVector2 IntVector2::DOWN = IntVector2(0, -1);
const IntVector2 IntVector2::LEFT = IntVector2(1, 0);
const IntVector2 IntVector2::RIGHT = IntVector2(-1, 0);
const IntVector2 IntVector2::ZERO = IntVector2(0, 0);
const IntVector2 IntVector2::ONE = IntVector2(1, 1);

//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( const IntVector2& copy )	
{
	x = copy.x;
	y = copy.y;
}


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( int initialX, int initialY )
{
	x = initialX;
	y = initialY;
}

IntVector2::IntVector2(const Vector2& vector)
{
	x = (int)vector.x;
	y = (int)vector.y;
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator + ( const IntVector2& vecToAdd ) const
{
	return IntVector2( x + vecToAdd.x, y + vecToAdd.y); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator-( const IntVector2& vecToSubtract ) const
{
	return IntVector2( x - vecToSubtract.x, y - vecToSubtract.y ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator*( int uniformScale ) const
{
	return IntVector2( x * uniformScale, y * uniformScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator/( int inverseScale ) const
{
	return IntVector2( x/inverseScale, y/inverseScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator+=( const IntVector2& vecToAdd )
{
	x = x + vecToAdd.x; // #MP1Fixme
	y = y + vecToAdd.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator-=( const IntVector2& vecToSubtract )
{
	x = x - vecToSubtract.x; // #MP1Fixme
	y = y - vecToSubtract.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator*=( const int uniformScale )
{
	x = x * uniformScale; // #MP1Fixme
	y = y * uniformScale; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator/=( const int uniformDivisor )
{
	x = x / uniformDivisor; // #MP1Fixme
	y = y / uniformDivisor; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator=( const IntVector2& copyFrom )
{
	x = copyFrom.x; // #MP1Fixme
	y = copyFrom.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator==( const IntVector2& compare ) const
{
	bool isEqual = false;

	if(x == compare.x && y == compare.y)
		isEqual = true;

	return isEqual;
}

//-----------------------------------------------------------------------------------------------
bool IntVector2::operator!=( const IntVector2& compare ) const
{
	bool isNotEqual = true;

	if(x == compare.x && y == compare.y)
		isNotEqual = false;

	return isNotEqual;
}

bool IntVector2::operator<(const IntVector2 & compare) const
{
	return false;
}

IntVector2 operator*( int uniformScale, const IntVector2& vecToScale )
{
	return IntVector2( vecToScale.x * uniformScale, vecToScale.y * uniformScale); // #MP1Fixme
}

void IntVector2::SetFromText(const char* text)
{
	std::string str(text);	
	int delimiterIndex = (int)str.find(',');

	std::string xVal = str.substr(0, delimiterIndex);
	std::string yVal = str.substr(delimiterIndex + 1);
	x = atoi(xVal.c_str());
	y = atoi(yVal.c_str());

	return;	
}

int GetDistance(const IntVector2& start, const IntVector2& end)
{
	int dx = start.x - end.x;
	int dy = start.y - end.y;
	int returnFloat = sqrtf((dx * dx) + (dy * dy));
	return returnFloat;	
}

int GetDistanceSquared(const IntVector2& a, const IntVector2& b)
{
	int dx = a.x - b.x;
	int dy = a.y - b.y;

	return ((dx * dx) + (dy * dy));
}

int MoveRegularPolygonX(int centerX, IntVector2 movementVector)
{
	centerX += movementVector.x;

	return (int)centerX;
}
int MoveRegularPolygonY(int centerY, IntVector2 movementVector)
{
	centerY += movementVector.y;

	return (int)centerY;
}

const IntVector2 Interpolate(const IntVector2& start, const IntVector2& end, float fractionTowardEnd)
{
	return IntVector2(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd));
}

int GetLargerOfXY(const IntVector2& vector)
{
	if(vector.y > vector.x)
	{
		return vector.y;
	}

	return vector.x;
}
