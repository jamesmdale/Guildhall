#pragma once
#include "Vector2.hpp"

//Axis-Algined Bounding Box 2D
class AABB2
{
public:
	~AABB2() {};									// destructor: do nothing (for speed)
	AABB2() {};
	AABB2(const AABB2& copyFrom);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(const Vector2& mins, const Vector2& maxs);
	explicit AABB2(const Vector2& center, float radiusX, float radiusY);
	explicit AABB2(const AABB2& bounds, const Vector2& minsPercentage, const Vector2& maxsPercentage);

	void StretchToIncludePoint(float x, float y);  //expand radius if (x,y) is outside
	void StretchToIncludePoint(const Vector2& point); //expand radius if point outside
	void AddPaddingToSides(float paddingRadiusX, float paddingRadiusY);
	void Translate(const Vector2& translation); //move the center
	void Translate(float translationX, float translationY); //move the center
	void AddPaddingToRadius( float paddingRadius );

	bool IsPointInside(float x, float y) const; //is (x,y) within disc's interior?
	bool IsPointInside(const Vector2& point) const; //is "point" within disc's interior?
	
	Vector2 GetDimensions() const; //return a vector2 of (width, height)
	Vector2 GetCenter() const; //return center position of the box
	void SetFromText(const char* text);

	Vector2 GetRandomPointInBounds();

	void operator+=(const Vector2& translation); //move (translate) the box
	void operator-=(const Vector2& antiTranslation);
	AABB2 operator+(const Vector2& translation) const; //create a (temp) moved box
	AABB2 operator-(const Vector2& antiTranslation) const;

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vector2 mins; //like Vector2 this breaks the "no public members and "m_" naming rules
	Vector2 maxs;

	static const AABB2 ZERO_TO_ONE;
};

bool DoAABBsOverlap(const AABB2& box1, const AABB2& box2); // determine if boxes overlap
const AABB2 Interpolate(const AABB2& start, const AABB2& end, float fractionTowardEnd);