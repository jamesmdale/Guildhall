#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Disc2.hpp"

class PointOfInterest
{
public:
	PointOfInterest(const Vector2& position, float radius);
	~PointOfInterest();

	void Update();
	void Render();

	//later add types and stuff

public:
	Vector2 m_position;
	Disc2 m_boundsDisc;
};

