#include "Engine\Math\AABB3.hpp"
#include "Engine\Math\Vector3.hpp"
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include <vector>

const AABB3 AABB3::ZERO_TO_ONE = AABB3(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f));

AABB3::AABB3( const AABB3& copy )	
{
	mins = copy.mins;
	maxs = copy.maxs;
}

AABB3::AABB3(const Vector3& minsVector, const Vector3& maxsVector)
{
	mins = minsVector;
	maxs = maxsVector;
}

AABB3::AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ)
{
	mins.x = center.x - radiusX;
	mins.y = center.y - radiusY;
	mins.z = center.z - radiusZ;

	maxs.x = center.x + radiusX;
	maxs.y = center.y + radiusY;
	maxs.z = center.z + radiusZ;
}

void AABB3::StretchToIncludePoint(float x, float y, float z)  //expand radius if (x,y) is outside
{
	//x
	if(x > maxs.x)
	{
		maxs.x = x;
	}
	if(x < mins.x)
	{
		mins.x = x;
	}

	//y
	if(y > maxs.y) 
	{
		maxs.y = y;
	}
	if(y < mins.y)
	{
		mins.y = y;
	}


	//z
	if(z > maxs.z) 
	{
		maxs.z = z;
	}
	if(z < mins.z)
	{
		mins.z = z;
	}
}
void AABB3::StretchToIncludePoint(const Vector3& point) //expand radius if point outside
{
	//x
	if(point.x > maxs.x)
	{
		maxs.x = point.x;
	}
	if(point.x < mins.x)
	{
		mins.x = point.x;
	}

	//y
	if(point.y > maxs.y) 
	{
		maxs.y = point.y;
	}
	if(point.y < mins.y)
	{
		mins.y = point.y;
	}

	//z
	if(point.z > maxs.z) 
	{
		maxs.z = point.z;
	}
	if(point.z < mins.z)
	{
		mins.z = point.z;
	}
}
void AABB3::AddPaddingToSides(float paddingRadiusX, float paddingRadiusY, float paddingRadiusZ)
{
	mins.x -= paddingRadiusX;	
	mins.y -= paddingRadiusY;
	mins.z -= paddingRadiusZ;

	maxs.x += paddingRadiusX;
	maxs.y += paddingRadiusY;
	maxs.z += paddingRadiusZ;
}
void AABB3:: Translate(const Vector3& translation) //move the center
{
	mins.x += translation.x;
	mins.y += translation.y;
	mins.z += translation.z;

	maxs.x += translation.x;
	maxs.y += translation.y;
	maxs.z += translation.z;
}
void AABB3::Translate(float translationX, float translationY, float translationZ) //move the center
{
	mins.x += translationX;
	mins.y += translationY;
	mins.z += translationZ;

	maxs.x += translationX;
	maxs.y += translationY;
	maxs.z += translationZ;
}
void AABB3::AddPaddingToRadius( float paddingRadius )
{
	mins.x -= paddingRadius;	
	mins.y -= paddingRadius;
	mins.z -= paddingRadius;

	maxs.x += paddingRadius;
	maxs.y += paddingRadius;
	maxs.z += paddingRadius;
}

bool AABB3::IsPointInside(float x, float y, float z) const //is (x,y) within AABB3's interior?
{
	bool isPointInside = false;
	if(x < maxs.x && x > mins.x && y < maxs.y && y > mins.y && z < maxs.z && z > mins.z)
	{
		isPointInside = true;
	}

	return isPointInside;
}
bool AABB3::IsPointInside(const Vector3& point) const //is "point" within AABB3's interior?
{
	bool isPointInside = false;
	if(point.x < maxs.x && point.x > mins.x && point.y < maxs.y && point.y > mins.y && point.z < maxs.z && point.z > mins.z)
	{
		isPointInside = true;
	}

	return isPointInside;
}

bool AABB3::DoesOverlapWithSphere(const Vector3 & sphereCenter, float sphereRadius)
{
	//easy out if box scenter is inside the sphere
	if (GetDistanceSquared(sphereCenter, GetCenter()) < sphereRadius * sphereRadius)
	{
		//	return true;
	}	

	Vector3 displacement = sphereCenter - GetCenter();
	Vector3 closestPointOnBox;
	Vector3 boxDimensions = GetDimensions();

	// check x =========================================================================================
	//sphere to is to the left of our box
	if (displacement.x < -boxDimensions.x * 0.5f)
	{
		closestPointOnBox.x = -boxDimensions.x * 0.5f;
	}	
	//sphere is to the right of our box
	else if (displacement.x > boxDimensions.x * 0.5f)
	{
		closestPointOnBox.x = boxDimensions.x * 0.5f;
	}
	else
	{
		closestPointOnBox.x = displacement.x;
	}

	// check y =========================================================================================
	//sphere to is below of our box
	if (displacement.y < -boxDimensions.y * 0.5f)
	{
		closestPointOnBox.y = -boxDimensions.y * 0.5f;
	}
	//sphere is above of our box
	else if (displacement.y > boxDimensions.y * 0.5f)
	{
		closestPointOnBox.y = boxDimensions.y * 0.5f;
	}
	else
	{
		closestPointOnBox.y = displacement.y;
	}

	// check z =========================================================================================
	//sphere is in front of our box
	if (displacement.z < -boxDimensions.z * 0.5f)
	{
		closestPointOnBox.z = -boxDimensions.z * 0.5f;
	}
	//sphere is behind of our box
	else if (displacement.z > boxDimensions.z * 0.5f)
	{
		closestPointOnBox.z = boxDimensions.z * 0.5f;
	}	
	else
	{
		closestPointOnBox.z = displacement.z;
	}

	
	float distanceSquared = GetDistanceSquared(displacement, closestPointOnBox);
	
	if (distanceSquared < sphereRadius * sphereRadius)
	{
		return true;
	}

	return false;
}

bool DoAABBsOverlap(const AABB3& box1, const AABB3& box2) // determine if boxes overlap
{
	bool doBoxesOverlap = true;

	//mins
	if(box1.mins.x > box2.maxs.x)
	{
		doBoxesOverlap = false;
	}
	if(box1.mins.y > box2.maxs.y)
	{
		doBoxesOverlap = false;
	}
	if (box1.mins.z > box2.maxs.z)
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
	if (box1.maxs.z < box2.mins.z)
	{
		doBoxesOverlap = false;
	}

	return doBoxesOverlap;
}

Vector3 AABB3::GetDimensions() const //return a Vector3 of (width, height)
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 dimensions = Vector3(width, height, depth);

	return dimensions;
}
Vector3 AABB3::GetCenter() const //return center position of the box
{
	Vector3 centerPoint = (mins + maxs) * .5;

	return centerPoint;
}

void AABB3::operator+=(const Vector3& translation) //move
{
	mins.x += translation.x;
	mins.y += translation.y;
	mins.z += translation.z;

	maxs.x += translation.x;
	maxs.y += translation.y;
	maxs.z += translation.z;

}

void AABB3::operator-=(const Vector3& translation) //move
{
	mins.x -= translation.x;
	mins.y -= translation.y;
	mins.z -= translation.z;

	maxs.x -= translation.x;
	maxs.y -= translation.y;
	maxs.z -= translation.z;
}

AABB3 AABB3::operator+(const Vector3& translation) const //move
{
	AABB3 tempBox = AABB3(mins, maxs);
	tempBox.mins.x += translation.x;
	tempBox.mins.y += translation.y;
	tempBox.mins.z += translation.z;

	tempBox.maxs.x += translation.x;
	tempBox.maxs.y += translation.y;
	tempBox.maxs.z += translation.z;

	return tempBox;
}

AABB3 AABB3::operator-(const Vector3& translation) const //move
{
	AABB3 tempBox = AABB3(mins, maxs);
	tempBox.mins.x -= translation.x;
	tempBox.mins.y -= translation.y;
	tempBox.mins.z -= translation.z;

	tempBox.maxs.x -= translation.x;
	tempBox.maxs.y -= translation.y;
	tempBox.maxs.z -= translation.z;

	return tempBox;
}

const AABB3 Interpolate(const AABB3& start, const AABB3& end, float fractionTowardEnd)
{
	Vector3 lerpedMins = Vector3(Interpolate(start.mins.x, end.mins.x, fractionTowardEnd), Interpolate(start.mins.y, end.mins.y, fractionTowardEnd), Interpolate(start.mins.z, end.mins.z, fractionTowardEnd));
	Vector3 lerpedMaxs = Vector3(Interpolate(start.maxs.x, end.maxs.x, fractionTowardEnd), Interpolate(start.maxs.y, end.maxs.y, fractionTowardEnd), Interpolate(start.maxs.z, end.maxs.z, fractionTowardEnd));
	return AABB3(lerpedMins, lerpedMaxs);
}

void AABB3::SetFromText(const char* text)
{
	std::string str(text);	

	std::vector<std::string> splitString = SplitStringOnCharacter(str, ',');

	mins.x = (float)atof(splitString[0].c_str());
	mins.y = (float)atof(splitString[1].c_str());
	mins.z = (float)atof(splitString[2].c_str());

	maxs.x = (float)atof(splitString[3].c_str());
	maxs.y = (float)atof(splitString[4].c_str());
	maxs.z = (float)atof(splitString[5].c_str());

	return;	
}