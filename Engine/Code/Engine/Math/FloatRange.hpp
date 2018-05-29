#pragma once
#include <math.h>
#include "engine\math\MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
class FloatRange
{
public:
	// Construction/Destruction
	~FloatRange() {}											// destructor: do nothing (for speed)
	FloatRange() {}											// default constructor: do nothing (for speed)
	FloatRange( const FloatRange& copyFrom );						// copy constructor (from another vec2)
	explicit FloatRange( float min, float max );		// explicit constructor (from min, max)												
	explicit FloatRange(float initialMinMax);

	float GetRandomInRange() const;
	void SetFromText(const char* text);
public: 
	float min;
	float max;
};

bool DoRangesOverlap(const FloatRange& a, const FloatRange& b);
const FloatRange Interpolate(const FloatRange& start, const FloatRange& end, float fractionTowardEnd);