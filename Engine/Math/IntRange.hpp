#pragma once
#include <math.h>
#include "engine\math\MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
class IntRange
{
public:
	// Construction/Destruction
	~IntRange() {}											// destructor: do nothing (for speed)
	IntRange() {}											// default constructor: do nothing (for speed)
	IntRange( const IntRange& copyFrom );						// copy constructor (from another vec2)
	explicit IntRange( int min, int max );		// explicit constructor (from min, max)												
	explicit IntRange(int initialMinMax);

	int GetRandomInRange() const;
	void SetFromText(const char* text);
public: 
	int min;
	int max;
};

bool DoRangesOverlap(const IntRange& a, const IntRange& b);
const IntRange Interpolate(const IntRange& start, const IntRange& end, float fractionTowardEnd);

