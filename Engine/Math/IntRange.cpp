#include "Engine\Math\IntRange.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
IntRange::IntRange( const IntRange& copy )	
{
	min = copy.min;
	max = copy.max;
}


//-----------------------------------------------------------------------------------------------
IntRange::IntRange( int inMin, int inMax )
{
	min = inMin;
	max = inMax;
}

IntRange::IntRange( int initialMinMax )
{
	min = initialMinMax;
	max = initialMinMax;
}

int IntRange::GetRandomInRange() const
{
	return GetRandomIntInRange(min, max);
}

void IntRange::SetFromText(const char* text)
{
	std::string str(text);	
	int delimiterIndex = (int)str.find('~');
	if(delimiterIndex == -1)
	{
		std::string minVal = str.substr(0, str.length());
		min = atoi(minVal.c_str());
		max = atoi(minVal.c_str());
		return;
	}
	std::string minVal = str.substr(0, delimiterIndex);
	std::string maxVal = str.substr(delimiterIndex + 1);
	max = atoi(maxVal.c_str());
	min = atoi(minVal.c_str());

	return;	
}



bool DoRangesOverlap(const IntRange& a, const IntRange& b)
{
	bool doesOverlap = false;
	if(a.min <= b.max && b.min <= a.max)
	{
		doesOverlap = true;
	}

	return doesOverlap;
}

const IntRange Interpolate(const IntRange& start, const IntRange& end, float fractionTowardEnd)
{
	return IntRange(Interpolate(start.min, end.min, fractionTowardEnd), Interpolate(start.max, end.max, fractionTowardEnd));
}