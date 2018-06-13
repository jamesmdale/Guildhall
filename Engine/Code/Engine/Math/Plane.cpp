#include "Engine\Math\Plane.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\MathUtils.hpp"


Plane::Plane()
{
}

Plane::Plane(const Vector3& newNormal, const Vector3 & newPosition)
{
	distance = DotProduct(newNormal, newPosition);
	normal = newNormal;
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	//counter clockwise for a normal to point toward us. (same as rendering)
	Vector3 e0 = b - a;
	Vector3 e1 = c - a;

	Vector3 normal = CrossProduct(e1, e0);

	ASSERT_OR_DIE(!IsNearZero(normal.GetLength()), "INVALID PLANE NORMAL! PLANE NORMAL = 0");

	normal.Normalize();
	distance = DotProduct(normal, a);
}


Plane::~Plane()
{
}

float Plane::GetDistanceFromPlane(const Vector3& positionToCompare) const
{
	float positionDistance = DotProduct(positionToCompare, normal);
	return positionDistance - distance; //relative to plane's distance from origin
}

bool Plane::IsInFront(const Vector3& positionToCompare) const
{
	//if n dot p < d = behind the plane
	//if n dot p > d = in front of the plane
	return GetDistanceFromPlane(positionToCompare) > 0.0f;
}

void Plane::FlipNormal()
{
	//negate normal and distance
	normal = -1.f * normal;
	distance = -1.f * distance;
}

RayCastHit3 RayCheckPlane(const Ray3 & ray, const Plane & plane)
{
	Vector3 firstPoint = ray.Evaluate(0.0f);
	Vector3 secondPoint = ray.Evaluate(1.0f);

	//ifnear just checks if they are really close
	if(IsNear(plane.GetDistanceFromPlane(firstPoint), plane.GetDistanceFromPlane(secondPoint)))
	{
		return RayCastHit3();	//i didn't hit. a default constructor says I didn't hit.
	}

	float direction = DotProduct(plane.normal, ray.direction);
	float distance = plane.GetDistanceFromPlane(ray.start);

	if ((direction * distance) > 0.0f)
	{
		return RayCastHit3();	//i didn't hit. a default constructor says I didn't hit.
	}

	float velocity = DotProduct(ray.direction, plane.normal);
	float time = (-1.f * distance) / velocity;

	return RayCastHit3(true, ray.Evaluate(time), plane.normal);
}

bool DoesSegmentIntersectPlane(const Segment3& segment, const Plane& plane)
{
	float distanceToStart = plane.GetDistanceFromPlane(segment.startPosition);
	float distanceToEnd = plane.GetDistanceFromPlane(segment.endPosition);

	//we only care about the sign.  if the sign is positive. It doesn't intersect. If the value is 0. It's on the plane and intersects. If it's negative it intersects
	return (distanceToStart * distanceToEnd) <= 0.0f; 
}
