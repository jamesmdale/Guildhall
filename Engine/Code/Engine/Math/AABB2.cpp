#include "engine\math\AABB2.hpp"
#include "engine\math\Vector2.hpp"
#include <string>
#include "engine/core/StringUtils.hpp"
#include <vector>

const AABB2 AABB2::ZERO_TO_ONE = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f));

AABB2::AABB2( const AABB2& copy )	
{
	mins = copy.mins;
	maxs = copy.maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	mins = Vector2(minX, minY);
	maxs = Vector2(maxX, maxY);
}

AABB2::AABB2(const Vector2& minsVector, const Vector2& maxsVector)
{
	mins = minsVector;
	maxs = maxsVector;
}

AABB2::AABB2(const Vector2& center, float radiusX, float radiusY)
{
	mins.x = center.x - radiusX;
	mins.y = center.y - radiusY;
	maxs.x = center.x + radiusX;
	maxs.y = center.y + radiusY;
}

AABB2::AABB2(const AABB2& bounds, const Vector2& minsPercentage, const Vector2& maxsPercentage)
{
	Vector2 dimensions = bounds.GetDimensions();
	
	Vector2 minsOffset = dimensions * minsPercentage;
	Vector2 maxsOffset = dimensions * maxsPercentage;

	mins = minsOffset + bounds.mins;
	maxs = maxsOffset + bounds.mins;
}

void AABB2::StretchToIncludePoint(float x, float y)  //expand radius if (x,y) is outside
{
	
	if(x > maxs.x)
	{
		maxs.x = x;
	}
	if(x < mins.x)
	{
		mins.x = x;
	}
	if(y > maxs.y) 
	{
		maxs.y = y;
	}
	if(y < mins.y)
	{
		mins.y = y;
	}
}
void AABB2::StretchToIncludePoint(const Vector2& point) //expand radius if point outside
{
	if(point.x > maxs.x)
	{
		maxs.x = point.x;
	}
	if(point.x < mins.x)
	{
		mins.x = point.x;
	}
	if(point.y > maxs.y) 
	{
		maxs.y = point.y;
	}
	if(point.y < mins.y)
	{
		mins.y = point.y;
	}
}
void AABB2::AddPaddingToSides(float paddingRadiusX, float paddingRadiusY)
{
	mins.x -= paddingRadiusX;	
	mins.y -= paddingRadiusY;

	maxs.x += paddingRadiusX;
	maxs.y += paddingRadiusY;
}
void AABB2:: Translate(const Vector2& translation) //move the center
{
	mins.x += translation.x;
	mins.y += translation.y;

	maxs.x += translation.x;
	maxs.y += translation.y;
}
void AABB2::Translate(float translationX, float translationY) //move the center
{
	mins.x += translationX;
	mins.y += translationY;

	maxs.x += translationX;
	maxs.y += translationY;
}
void AABB2::AddPaddingToRadius( float paddingRadius )
{
	mins.x -= paddingRadius;	
	mins.y -= paddingRadius;

	maxs.x += paddingRadius;
	maxs.y += paddingRadius;
}

bool AABB2::IsPointInside(float x, float y) const //is (x,y) within AABB2's interior?
{
	bool isPointInside = false;
	if(x < maxs.x && x > mins.x && y < maxs.y && y > mins.y)
	{
		isPointInside = true;
	}

	return isPointInside;
}
bool AABB2::IsPointInside(const Vector2& point) const //is "point" within AABB2's interior?
{
	bool isPointInside = false;
	if(point.x < maxs.x && point.x > mins.x && point.y < maxs.y && point.y > mins.y)
	{
		isPointInside = true;
	}

	return isPointInside;
}

bool DoAABBsOverlap(const AABB2& box1, const AABB2& box2) // determine if boxes overlap
{
	bool doBoxesOverlap = true;
	if(box1.mins.x > box2.maxs.x)
	{
		doBoxesOverlap = false;
	}
	if(box1.mins.y > box2.maxs.y)
	{
		doBoxesOverlap = false;
	}
	if(box1.maxs.x < box2.mins.x)
	{
		doBoxesOverlap = false;
	}
	if(box1.maxs.y < box2.mins.y)
	{
		doBoxesOverlap = false;
	}	

	return doBoxesOverlap;
}

Vector2 AABB2::GetDimensions() const //return a vector2 of (width, height)
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	Vector2 dimensions = Vector2(width, height);

	return dimensions;
}
Vector2 AABB2::GetCenter() const //return center position of the box
{
	Vector2 centerPoint = (mins + maxs) * .5;

	return centerPoint;
}

Vector2 AABB2::GetRandomPointInBounds()
{
	return Vector2(GetRandomFloatInRange(mins.x, maxs.x), GetRandomFloatInRange(mins.y, maxs.y));
}

void AABB2::operator+=(const Vector2& translation) //move
{
	mins.x += translation.x;
	mins.y += translation.y;

	maxs.x += translation.x;
	maxs.y += translation.y;
}

void AABB2::operator-=(const Vector2& translation) //move
{
	mins.x -= translation.x;
	mins.y -= translation.y;

	maxs.x -= translation.x;
	maxs.y -= translation.y;
}

AABB2 AABB2::operator+(const Vector2& translation) const //move
{
	AABB2 tempBox = AABB2(mins, maxs);
	tempBox.mins.x += translation.x;
	tempBox.mins.y += translation.y;

	tempBox.maxs.x += translation.x;
	tempBox.maxs.y += translation.y;

	return tempBox;
}

AABB2 AABB2::operator-(const Vector2& translation) const //move
{
	AABB2 tempBox = AABB2(mins, maxs);
	tempBox.mins.x -= translation.x;
	tempBox.mins.y -= translation.y;

	tempBox.maxs.x -= translation.x;
	tempBox.maxs.y -= translation.y;

	return tempBox;
}

const AABB2 Interpolate(const AABB2& start, const AABB2& end, float fractionTowardEnd)
{
	Vector2 lerpedMins = Vector2(Interpolate(start.mins.x, end.mins.x, fractionTowardEnd), Interpolate(start.mins.y, end.mins.y, fractionTowardEnd));
	Vector2 lerpedMaxs = Vector2(Interpolate(start.maxs.x, end.maxs.x, fractionTowardEnd), Interpolate(start.maxs.y, end.maxs.y, fractionTowardEnd));
	return AABB2(lerpedMins, lerpedMaxs);
}

void AABB2::SetFromText(const char* text)
{
	std::string str(text);	

	std::vector<std::string> splitString = SplitStringOnCharacter(str, ',');

	mins.x = (float)atof(splitString[0].c_str());
	mins.y = (float)atof(splitString[1].c_str());
	maxs.x = (float)atof(splitString[2].c_str());
	maxs.y = (float)atof(splitString[3].c_str());

	return;	
}