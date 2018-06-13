#include "Engine\Math\Segment3.hpp"



Segment3::Segment3()
{
}

Segment3::Segment3(const Vector3 & newStartPos, const Vector3 & newEndPos)
{
	startPosition = newStartPos;
	endPosition = newEndPos;
}


Segment3::~Segment3()
{
}
