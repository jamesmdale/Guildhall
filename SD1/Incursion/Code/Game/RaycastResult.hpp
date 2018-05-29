#pragma once
#include "Engine/math/Vector2.hpp"
#include "Engine/math/IntVector2.hpp"
struct RaycastResult
{
	bool didCollide;
	Vector2 collisionPosition;
	IntVector2 collisionTile;
	float fractionOfImpact;
	Vector2 impactNormal;
};