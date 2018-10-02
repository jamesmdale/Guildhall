#include "engine\math\Disc2.hpp"
#include "engine\math\AABB2.hpp"

Disc2::Disc2( const Disc2& copy )	
{
	center = copy.center;
	radius = copy.radius;
}
Disc2::Disc2(float initialX, float initialY, float initialRadius)
{
	center = Vector2(initialX, initialY);
	radius = initialRadius;
}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius)
{
	center = initialCenter;
	radius = initialRadius;
}

void Disc2::StretchToIncludePoint(float x, float y)
{
	Vector2 includedVector = Vector2(x, y);
	float distance = GetDistance(includedVector, center);
	
	if(radius < distance)
	{
		radius = distance;
	}

}

void Disc2::StretchToIncludePoint(const Vector2& point) //expand radius if point outside
{
	float distance = GetDistance(point, center);

	if(radius < distance)
	{
		radius = distance;
	}
}

void Disc2::AddPaddingToRaidus(float paddingRadius)
{
	radius += paddingRadius;
}

void Disc2::Translate(const Vector2& translation) //move the center
{
	center.x += translation.x;
	center.y += translation.y;
}

void Disc2::Translate(float translationX, float translationY) //move the center
{
	center.x += translationX;
	center.y += translationY;
}

void Disc2::AddPaddingToRadius( float paddingRadius )
{
	radius += paddingRadius;
}

bool Disc2::IsPointInside(float x, float y) const //is (x,y) within disc's interior?
{
	bool isPointInside = false;

	Vector2 vectorToCheck = Vector2(x, y);
	float distance = GetDistance(vectorToCheck, center);

	if(radius > distance)
	{
		isPointInside = true;
	}

	return isPointInside;
}
bool Disc2::IsPointInside(const Vector2& point) const //is "point" within disc's interior?
{
	bool isPointInside = false;

	float distance = GetDistance(point, center);

	if(radius > distance)
	{
		isPointInside = true;
	}

	return isPointInside;

}

void Disc2::operator+=(const Vector2& translation) //move
{
	center.x += translation.x;
	center.y += translation.y;

}

void Disc2::operator-=(const Vector2& antiTranslation)
{
	center.x -= antiTranslation.x;
	center.y -=  antiTranslation.y;
}

Disc2 Disc2::operator+(const Vector2& translation) const
{
	Disc2 tempDisc = Disc2(center.x, center.y, radius);

	tempDisc.center.x += translation.x;
	tempDisc.center.y += translation.y;

	return tempDisc;
}

Disc2 Disc2::operator-(const Vector2& antiTranslation) const
{
	Disc2 tempDisc = Disc2(center.x, center.y, radius);

	tempDisc.center.x -= antiTranslation.x;
	tempDisc.center.y -= antiTranslation.y;

	return tempDisc;

}

void Disc2::operator=( const Disc2& copyFrom )
{
	center = copyFrom.center;
	radius = copyFrom.radius;
}


bool DoDiscsOverlap(const Disc2& a, const Disc2& b)
{
	bool doesOverlap = false;
	float distance = GetDistance(a.center, b.center);
	float radiSum = a.radius + b.radius;

	if(distance < radiSum)
		doesOverlap = true;

	return doesOverlap;
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	bool doesOverlap = false;
	float distance = GetDistance(aCenter, bCenter);
	float radiSum = aRadius + bRadius;

	if(distance < radiSum)
		doesOverlap = true;

	return doesOverlap;

}

bool DoesDiscOverlapWithAABB2(const Disc2& disc, const AABB2& box)
{
	bool doesOverlap = false;

	Vector2 directionToCenterVector = box.GetCenter() - disc.center;
	float length = directionToCenterVector.NormalizeAndGetLength();

	Vector2 positionToCheck = (directionToCenterVector * disc.radius) + disc.center;
	
	if(box.IsPointInside(positionToCheck))
		doesOverlap = true;

	return doesOverlap;
}

const Disc2 Interpolate(const Disc2& start, const Disc2& end, float fractionTowardEnd)
{
	Vector2 lerpedCenter = Vector2(Interpolate(start.center.x, end.center.x, fractionTowardEnd), Interpolate(start.center.y, end.center.y, fractionTowardEnd));
	float lerpedRadius = Interpolate(start.radius, end.radius, fractionTowardEnd);

	return Disc2(lerpedCenter, lerpedRadius);
}