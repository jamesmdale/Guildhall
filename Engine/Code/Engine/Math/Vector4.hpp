#pragma once
#include <math.h>
#include "engine\math\MathUtils.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Rgba.hpp"

class Vector3;
//-----------------------------------------------------------------------------------------------
class Vector4
{
public:
	// Construction/Destruction
	Vector4();											// default constructor: do nothing (for speed)
	~Vector4() {}											// destructor: do nothing (for speed)
												
	Vector4( const Vector4& copyFrom );						// copy constructor (from another vec2)
	explicit Vector4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y)
	explicit Vector4( Vector3 vec, float appendedW);
	explicit Vector4(Rgba color);

	void SetFromText(const char * text);

	float GetLength() const;
	float Dot(const Vector4& dotVector);
	float NormalizeAndGetLength();
	Vector4 GetNormalized() const;
	void Normalize();

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	float z;
	float w;

	static const Vector4 IDENTITY_POINT_TRANSLATION;
	static const Vector4 IDENTITY_VECTOR_TRANSLATION;

	static const Vector4 ZERO;
	static const Vector4 ONE;
};

const Vector4 Interpolate(const Vector4& start, const Vector4& end, float fractionTowardEnd);