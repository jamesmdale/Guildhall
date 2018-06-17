#pragma once
#include "Engine\Math\Vector3.hpp"

struct Ray3
{
	Ray3(){};
	Ray3(const Vector3& startPosition, const Vector3& rayDirection)
	{
		start = startPosition;
		direction = rayDirection;
	}

	Vector3 Evaluate(float t) const
	{
		return start + (direction * t);
	}

	// members 
	Vector3 start;
	Vector3 direction;
};

struct RayCastHit3
{
	RayCastHit3(){};
	RayCastHit3(bool didHit, const Vector3& hitPosition, const Vector3& normalAtHit)
	{
		hit = didHit;
		position = hitPosition;
		normal = normalAtHit;
	}

	// members 
	bool hit = false;
	Vector3 position;
	Vector3 normal;
};