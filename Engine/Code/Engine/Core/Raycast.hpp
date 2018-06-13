#pragma once
#include "Engine\Math\Vector3.hpp"

struct Ray3
{
	Vector3 start;
	Vector3 direction;

	Vector3 Evaluate(float t) const
	{
		return start + (direction * t);
	}
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

	bool hit;
	Vector3 position;
	Vector3 normal;
};