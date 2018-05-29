#pragma once
#include <math.h>
#include "engine\math\MathUtils.hpp"
#include "Engine\Math\IntVector2.hpp"

//-----------------------------------------------------------------------------------------------
class IntVector3
{
public:
	// Construction/Destruction
	~IntVector3() {}											// destructor: do nothing (for speed)
	IntVector3() {}											// default constructor: do nothing (for speed)
	IntVector3( const IntVector3& copyFrom );						// copy constructor (from another vec2)
	IntVector3(const IntVector2& initialXY, int initialZ);
	explicit IntVector3( int initialX, int initialY, int initialZ );		// explicit constructor (from x, y)

															// Operators
	const IntVector3 operator+( const IntVector3& vecToAdd ) const;		// vec2 + vec2
	const IntVector3 operator-( const IntVector3& vecToSubtract ) const;	// vec2 - vec2
	const IntVector3 operator*( int uniformScale ) const;			// vec2 * int
	const IntVector3 operator/( int inverseScale ) const;			// vec2 / int
	void operator+=( const IntVector3& vecToAdd );						// vec2 += vec2
	void operator-=( const IntVector3& vecToSubtract );				// vec2 -= vec2
	void operator*=( const int uniformScale );					// vec2 *= int
	void operator/=( const int uniformDivisor );					// vec2 /= int
	void operator=( const IntVector3& copyFrom );						// vec2 = vec2
	bool operator==( const IntVector3& compare ) const;				// vec2 == vec2
	bool operator!=( const IntVector3& compare ) const;				// vec2 != vec2
	friend IntVector3 operator*( int uniformScale, const IntVector3& vecToScale);	// int * vec2

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;
	int z;

	static const IntVector3 ZERO;
	static const IntVector3 ONE;
	static const IntVector3 RIGHT;
	static const IntVector3 UP ;
	static const IntVector3 FORWARD;
};