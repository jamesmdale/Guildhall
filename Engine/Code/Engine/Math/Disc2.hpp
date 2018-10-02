#pragma  once
#include "engine\math\vector2.hpp"
#include <math.h>
#include "engine\math\MathUtils.hpp"
#include "engine\math\AABB2.hpp"

class Disc2
{
public:
	~Disc2() {};									// destructor: do nothing (for speed)
	Disc2() {};
	Disc2(const Disc2& copyFrom);
	explicit Disc2(float initialX, float initialY, float initialRadius);
	explicit Disc2(const Vector2& initialCenter, float initialRadius);

	void StretchToIncludePoint(float x, float y);  //expand radius if (x,y) is outside
	void StretchToIncludePoint(const Vector2& point); //expand radius if point outside
	void AddPaddingToRaidus(float paddingRadius);
	void Translate(const Vector2& translation); //move the center
	void Translate(float translationX, float translationY); //move the center
	void AddPaddingToRadius( float paddingRadius );

	bool IsPointInside(float x, float y) const; //is (x,y) within disc's interior?
	bool IsPointInside(const Vector2& point) const; //is "point" within disc's interior?

	void operator+=(const Vector2& translation); //move
	void operator-=(const Vector2& antiTranslation);
	Disc2 operator+(const Vector2& translation) const;
	Disc2 operator-(const Vector2& antiTranslation) const;
	void Disc2::operator=(const Disc2& copyFrom);

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vector2 center; //like Vector2 this breaks the "no public members and "m_" naming rules
	float radius;

};

bool DoDiscsOverlap(const Disc2& a, const Disc2& b);
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);
bool DoesDiscOverlapWithAABB2(const Disc2& disc, const AABB2& box);
const Disc2 Interpolate(const Disc2& start, const Disc2& end, float fractionTowardEnd);
