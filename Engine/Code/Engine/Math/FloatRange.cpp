#include "Engine\Math\FloatRange.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange( const FloatRange& copy )	
{
	min = copy.min;
	max = copy.max;
}


//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange( float inMin, float inMax )
{
	min = inMin;
	max = inMax;
}

FloatRange::FloatRange( float initialMinMax )
{
	min = initialMinMax;
	max = initialMinMax;
}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloatInRange(min, max);
}

void FloatRange::SetFromText(const char* text)
{
	std::string str(text);	
	int delimiterIndex = (int)str.find('~');
	if(delimiterIndex == -1)
	{
		std::string minVal = str.substr(0, str.length());
		min = (float)atof(minVal.c_str());
		max = (float)atof(minVal.c_str());
		return;
	}
	std::string minVal = str.substr(0, delimiterIndex);
	std::string maxVal = str.substr(delimiterIndex + 1);
	max = (float)atof(maxVal.c_str());
	min = (float)atof(minVal.c_str());

	return;	
}

bool DoRangesOverlap(const FloatRange& a, const FloatRange& b)
{
	bool doesOverlap = false;
	if(a.min <= b.max && b.min <= a.max)
	{
		doesOverlap = true;
	}

	return doesOverlap;
}

const FloatRange Interpolate(const FloatRange& start, const FloatRange& end, float fractionTowardEnd)
{
	return FloatRange(Interpolate(start.min, end.min, fractionTowardEnd), Interpolate(start.max, end.max, fractionTowardEnd));
}