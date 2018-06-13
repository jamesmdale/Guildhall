#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Segment3.hpp"
#include "Engine\Core\Raycast.hpp"

class Plane
{
public:
	Plane();
	Plane(const Vector3& newNormal, const Vector3& newPosition);
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	~Plane();

	float GetDistanceFromPlane(const Vector3& positionToCompare) const;
	bool IsInFront(const Vector3& positionToCompare) const;
	void FlipNormal();

public:
	Vector3 normal;
	float distance;
};

RayCastHit3 RayCheckPlane(const Ray3& ray, const Plane& plane);
bool DoesSegmentIntersectPlane(const Segment3& segment, const Plane& plane);

