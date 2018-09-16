#pragma once
#include <math.h>
#include "engine\math\MathUtils.hpp"

class Vector2;
//-----------------------------------------------------------------------------------------------
class IntVector2
{
public:
	// Construction/Destruction
	~IntVector2() {}											// destructor: do nothing (for speed)
	IntVector2() {}											// default constructor: do nothing (for speed)
	IntVector2( const IntVector2& copyFrom );						// copy constructor (from another vec2)
	explicit IntVector2( int initialX, int initialY );		// explicit constructor (from x, y)
	explicit IntVector2(const Vector2& vector);
															// Operators
	const IntVector2 operator+( const IntVector2& vecToAdd ) const;		// vec2 + vec2
	const IntVector2 operator-( const IntVector2& vecToSubtract ) const;	// vec2 - vec2
	const IntVector2 operator*( int uniformScale ) const;			// vec2 * int
	const IntVector2 operator/( int inverseScale ) const;			// vec2 / int
	void operator+=( const IntVector2& vecToAdd );						// vec2 += vec2
	void operator-=( const IntVector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const int uniformScale );					// vec2 *= int
	void operator/=( const int uniformDivisor );					// vec2 /= int
	void operator=( const IntVector2& copyFrom );						// vec2 = vec2
	bool operator==( const IntVector2& compare ) const;				// vec2 == vec2
	bool operator!=( const IntVector2& compare ) const;				// vec2 != vec2
	friend IntVector2 operator*( int uniformScale, const IntVector2& vecToScale);	// int * vec2
	void SetFromText(const char* text);

	bool operator<(const IntVector2& compare) const;

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;

	static const IntVector2 UP;
	static const IntVector2 DOWN;
	static const IntVector2 LEFT;
	static const IntVector2 RIGHT;
	static const IntVector2 ZERO;
	static const IntVector2 ONE;
};
int GetDistance(const IntVector2& start, const IntVector2& end);
int GetDistanceSquared(const IntVector2& a, const IntVector2& b);
const IntVector2 Interpolate(const IntVector2& start, const IntVector2& end, float fractionTowardEnd);
int GetLargerOfXY(const IntVector2& vector);
