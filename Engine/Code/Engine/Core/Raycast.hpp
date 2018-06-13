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
	bool hit;
	Vector3 position;
	Vector3 normal;
};