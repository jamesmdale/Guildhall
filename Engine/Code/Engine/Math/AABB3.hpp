#pragma once
#include "Engine\Math\Vector3.hpp"

//Axis-Algined Bounding Box 2D
class AABB3
{
public:
	~AABB3() {};									// destructor: do nothing (for speed)
	AABB3() {};
	AABB3(const AABB3& copyFrom);
	explicit AABB3(const Vector3& mins, const Vector3& maxs);
	explicit AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ);

	void StretchToIncludePoint(float x, float y, float z);  //expand radius if (x,y) is outside
	void StretchToIncludePoint(const Vector3& point); //expand radius if point outside
	void AddPaddingToSides(float paddingRadiusX, float paddingRadiusY, float paddingRadiusZ);
	void Translate(const Vector3& translation); //move the center
	void Translate(float translationX, float translationY, float translationZ); //move the center
	void AddPaddingToRadius( float paddingRadius );

	bool IsPointInside(float x, float y, float z) const; //is (x,y) within disc's interior?
	bool IsPointInside(const Vector3& point) const; //is "point" within disc's interior?
	bool DoesOverlapWithSphere(const Vector3& sphereCenter, float sphereRadius);

	Vector3 GetDimensions() const; //return a vector2 of (width, height)
	Vector3 GetCenter() const; //return center position of the box
	void SetFromText(const char* text);

	void operator+=(const Vector3& translation); //move (translate) the box
	void operator-=(const Vector3& antiTranslation);
	AABB3 operator+(const Vector3& translation) const; //create a (temp) moved box
	AABB3 operator-(const Vector3& antiTranslation) const;

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vector3 mins; //like Vector2 this breaks the "no public members and "m_" naming rules
	Vector3 maxs;

	static const AABB3 ZERO_TO_ONE;
};