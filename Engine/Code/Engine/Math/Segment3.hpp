#pragma once
#include "Engine\Math\Vector3.hpp"
class Segment3
{
public:
	Segment3();
	Segment3(const Vector3& newStartPos, const Vector3& newEndPos);
	~Segment3();
public:
	Vector3 startPosition;
	Vector3 endPosition;
};

