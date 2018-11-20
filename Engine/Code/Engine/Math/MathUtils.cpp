#include <math.h>
#include <stdlib.h>
#include <cmath>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\Matrix44.hpp"

constexpr float PI = static_cast<float>(3.14159);
constexpr float EPSILON = static_cast<float>(0.001);
constexpr float UINFINITY = INFINITY;

float ConvertDegreesToRadians(float degrees)
{
	float radians;

	radians = degrees * (PI/180);

	return (float)radians;
}

int GetRandomIntLessThan(int maxNotInclusive)
{
	if(maxNotInclusive == 0)
		return 0;

	int random = rand() % maxNotInclusive;
	return random;
}

unsigned int GetRandomUintLessThan(unsigned int maxNotInclusive)
{
	if(maxNotInclusive == 0)
		return 0;

	unsigned int random = rand() % maxNotInclusive;
	return random;
}

float GetRandomFloatLessThan()
{
	return static_cast <float> (rand()) /( static_cast <float> (RAND_MAX));
}

float GenerateRandomFloatInRangeLessThanOne(float min)
{
	float range = (1 - min);
	float randomFloat = GetRandomFloatLessThan();	 
	float number = (randomFloat * range) + min;
	return number;
}

float GetRandomFloatInRange(float min, float max)
{
	//improved get random flaot
	return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

int GetRandomIntInRange(int min, int max)
{
	int range = (max - min) + 1;
	return min + GetRandomIntLessThan(range);
}

unsigned int GetRandomUintInRange(unsigned int min, unsigned int max)
{
	unsigned int range = (max - min) + 1;
	return min + GetRandomUintLessThan(range);
}

Vector3 GetRandomVector3InSphere(float positionMin, float positionMax)
{
	Vector3 randomVector;

	randomVector.x = GetRandomFloatInRange(positionMin, positionMax);
	randomVector.y = GetRandomFloatInRange(positionMin, positionMax);
	randomVector.z = GetRandomFloatInRange(positionMin, positionMax);

	return randomVector;
}

float GetRandomFloatZeroToOne()
{
	return (float)rand() / (float) RAND_MAX;
}

float GetRandomFloatZeroOrOne()
{
	float val = GetRandomFloatZeroToOne();
	if(val >= 0.5f)
	{
		return 1.f;
	}

	return 0.f;
}

int GetRandomIntZeroOrOne()
{
	float val = GetRandomFloatZeroToOne();
	if(val >= 0.5f)
	{
		return 1;
	}

	return 0;
}

float CalculateXPoint(float centerX, float radius, float radians)
{
	float xPoint = centerX + (radius * cosf(radians));

	return xPoint;
}
float CalculateYPoint(float centerY, float radius, float radians)
{
	float yPoint = centerY + (radius * sinf(radians));

	return yPoint;
}

float GetMinFloat(float a, float b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

int GetMinInt(int a, int b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

int RotateRegularPolygon(float currentDegrees, float rotationDegrees)
{
	return (int)(currentDegrees + rotationDegrees);
}

float ConvertRadiansToDegrees(float radians)
{
	 return (float)(radians * (180.0/PI));
}

float CosDegrees(float degrees)
{
	return (cosf(ConvertDegreesToRadians(degrees)));
}

float TanDegrees(float degrees)
{
	return (tanf(ConvertDegreesToRadians(degrees)));
}

float SinDegrees(float degrees)
{
	return (sinf(ConvertDegreesToRadians(degrees)));
}

float ClampFloat(float inValue, float minInclusiveValue, float maxInclusiveValue)
{
	float clampedValue;
	if(inValue > maxInclusiveValue)
	{
		clampedValue = maxInclusiveValue;
	}
	else if(inValue < minInclusiveValue)
	{
		clampedValue = minInclusiveValue;
	}
	else
	{
		clampedValue = inValue;
	}	
	return clampedValue;
}

double ClampDouble(double inValue, double minInclusiveValue, double maxInclusiveValue)
{
	double clampedValue;
	if (inValue > maxInclusiveValue)
	{
		clampedValue = maxInclusiveValue;
	}
	else if (inValue < minInclusiveValue)
	{
		clampedValue = minInclusiveValue;
	}
	else
	{
		clampedValue = inValue;
	}
	return clampedValue;
}

Vector3 ClampVector3Uniform(Vector3 inValue, Vector3 minInclusiveValue, Vector3 maxInclusiveValue)
{
	inValue.x = ClampFloat(inValue.x, minInclusiveValue.x, maxInclusiveValue.x);
	inValue.y = ClampFloat(inValue.y, minInclusiveValue.y, maxInclusiveValue.y);
	inValue.z = ClampFloat(inValue.z, minInclusiveValue.z, maxInclusiveValue.z);

	return inValue;
}

Vector4 ClampVector4Uniform(Vector4 inValue, Vector4 minInclusiveValue, Vector4 maxInclusiveValue)
{
	inValue.x = ClampFloat(inValue.x, minInclusiveValue.x, maxInclusiveValue.x);
	inValue.y = ClampFloat(inValue.y, minInclusiveValue.y, maxInclusiveValue.y);
	inValue.z = ClampFloat(inValue.z, minInclusiveValue.z, maxInclusiveValue.z);
	inValue.w = ClampFloat(inValue.w, minInclusiveValue.w, maxInclusiveValue.w);

	return inValue;
}

int ClampInt(int inValue, int minInclusiveValue, int maxInclusiveValue)
{
	int clampedValue;
	if(inValue > maxInclusiveValue)
	{
		clampedValue = maxInclusiveValue;
	}
	else if(inValue < minInclusiveValue)
	{
		clampedValue = minInclusiveValue;
	}
	else
	{
		clampedValue = inValue;
	}	
	return clampedValue;
}

float ClampFloatZeroToOne(float inValue)
{
	float clampedValue = 0.f;
	if(inValue < 0.f)
	{
		clampedValue = 0.f;
	}
	else if(inValue > 1.f)
	{
		clampedValue = 1.f;
	}
	else
	{
		clampedValue = inValue;
	}
	return clampedValue;
}

float ClampFloatNegativeOneToOne( float inValue )
{
	float clampedValue;
	if(inValue < -1.f)
	{
		clampedValue = -1.f;
	}
	else if(inValue > 1.f)
	{
		clampedValue = 1.f;
	}
	else
	{
		clampedValue = inValue;
	}
	return clampedValue;
}

float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	if(inStart == inEnd)
	{
		return (outStart + outEnd) * 0.5f;
	}
	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inRelativeToStart = inValue - inStart;
	float fractionIntoRange = inRelativeToStart / inRange;
	float outRelativeToStart = fractionIntoRange * outRange;
	float outValue = outRelativeToStart + outStart;

	return outValue;
}

float AtanfAsDegrees(float y, float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

float AcosfAsDegrees(float x, float radius)
{
	return ConvertRadiansToDegrees(acosf(x/radius));
}

float AsinfAsDegrees(float y, float radius)
{
	return ConvertRadiansToDegrees(asinf(y/radius));
}

bool CheckRandomChance( float chanceForSuccess )
{
	bool isSuccess = false;
	float random = GetRandomFloatZeroToOne();

	if(chanceForSuccess > random)
	{
		isSuccess = true;
	}

	return isSuccess;
}

int RoundToNearestInt( float inValue )
{
	int roundedInt = (int)std::roundf(inValue);	
	return roundedInt;
}

float RoundDownToDecimalPlace(float inValue, int decimalPlace)
{
	return floorf(inValue * decimalPlace) / decimalPlace;
}

float TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees )
{
	float displacement = GetAngularDisplacement(currentDegrees, goalDegrees);
	//float movedDegrees = 0.0f;
	if(displacement < 0)
	{
		if(displacement >= maxTurnDegrees)
			return goalDegrees;
		else
			return currentDegrees -= maxTurnDegrees;
	}
	if(displacement > 0)
	{
		if(displacement <= maxTurnDegrees)
			return goalDegrees;
		else
			return currentDegrees += maxTurnDegrees;
	}

	return currentDegrees;
}

// Finds the % (as a fraction) of inValue in [rangeStart,rangeEnd].
// For example, 3 is 25% (0.25) of the way through the range [2,6].
float GetFractionInRange(float inValue, float rangeStart, float rangeEnd)
{
	float range = rangeEnd - rangeStart;
	float inRelativeToStart = inValue - rangeStart;

	float fractionIntoRange = inRelativeToStart / range;

	return fractionIntoRange;
}

// Finds the “angular displacement” (or signed angular distance) from startDegrees to endDegrees.
// This function always takes the smartest/shortest way around, so going from 5 to 355 yields -10.
float GetAngularDisplacement( float startDegrees, float endDegrees )
{
	float angularDisplacement = endDegrees - startDegrees;	

	while(angularDisplacement > 180)
	{			
		angularDisplacement -= 360;
	}
	while(angularDisplacement < -180)
	{
		angularDisplacement += 360;			
	}

	return angularDisplacement;
}

float DotProduct( const Vector2& a, const Vector2& b )
{
	float dotProduct = (a.x * b.x) + (a.y * b.y);
	return dotProduct;
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	float dotProduct = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	return dotProduct;
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	float dotProduct = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	return dotProduct;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	//EX: UP X FORWARD = RIGHT
	Vector3 c = Vector3((a.y * b.z) - (b.y * a.z), (a.z * b.x) - (b.z * a.x), (a.x * b.y) - (b.x * a.y)); 
	
	return c;
}

bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck )
{
	return (bitFlags8 & flagsToCheck) == flagsToCheck;
}

bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck )
{
	return (bitFlags32 & flagsToCheck) == flagsToCheck;
}

void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet )
{
	 bitFlags8 |= flagsToSet;
}

void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet )
{
	bitFlags32 |= flagsToSet;
}

void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear )
{
	unsigned char actualMask = ~flagToClear;
	bitFlags8 &= actualMask;
}

void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear )
{
	unsigned int actualMask = ~flagToClear;
	bitFlags32 &= actualMask;
}


IntVector2 ConvertVector2ToIntVector2(Vector2 vectorToConvert)
{
	return IntVector2((int)vectorToConvert.x, (int)vectorToConvert.y);
}

Vector3 ConvertVector2ToVector3(Vector2 vectorToConvert)
{
	return Vector3(vectorToConvert.x, vectorToConvert.y, 0);
}

//easing functions
float SmoothStart2( float t )
{
	return (t * t);

}
float SmoothStart3( float t )
{
	return (t * t * t);
}
float SmoothStart4( float t )
{
	return (t * t * t * t);
}
float SmoothStop2( float t )
{
	float flippedInput = 1 - t;
	
	return (1 - ( flippedInput * flippedInput));
}
float SmoothStop3( float t )
{
	float flippedInput = 1 - t;

	return (1 - (flippedInput * flippedInput * flippedInput));
}
float SmoothStop4( float t )
{
	float flippedInput = 1 - t;

	return (1 - (flippedInput * flippedInput * flippedInput * flippedInput));
}
float SmoothStep3( float t )
{
	return Crossfade(SmoothStart2(t), SmoothStop2(t), t);
}

float SmoothStartArch3(float t)
{
	return (t * t * (1-t));
}

//equivalent to GLSL smoothstep
float GLSLSmoothStep(float edge0, float edge1, float x)
{
	float value = (x - edge0) / (edge1 - edge0);
	float t = ClampFloatZeroToOne(value);
	return t * t * (3.0f - 2.0f * t);
}

float Crossfade(float a, float b, float t)
{
	return (a + t * (b - a));
}

// Finds the value at a certain % (fraction) in [rangeStart,rangeEnd].
// For example, at 75% (0.75) of the way through [2,6] is 5.
//NOTE: rewritten form of previous interpolate so can all be one line
const float Interpolate(const float& start, const float& end, float fractionTowardEnd)
{
	return (((end - start) * fractionTowardEnd) + start);
}


//  =============================================================================
bool CyclicLessThan(uint16_t a, uint16_t b)
{
	uint16_t diff = b - a;

	//same as
	//int16_t idiff = (int16_T)diff;
	//return idiff > 0

	return (diff > 0) && (diff <= 0x7fff);
}

//  =============================================================================
bool CyclicGreaterThan(uint16_t a, uint16_t b)
{
	return CyclicLessThan(b, a);
}


const int Interpolate(const int& start, const int& end, float fractionTowardEnd)
{
	return (RoundToNearestInt((end - start) * fractionTowardEnd) + start);
}


const unsigned char Interpolate(const unsigned char& start, const unsigned char& end, float fractionTowardEnd)
{
	return (unsigned char)(RoundToNearestInt((end - start) * fractionTowardEnd) + start);
}

float GetPi()
{
	return PI;
}

float GetEpsilon()
{
	return EPSILON;
}

float GetInfinity()
{
	return UINFINITY;
}

bool IsNearZero(float inValue)
{
	if (inValue < 0.0f)
	{
		if (inValue + GetEpsilon() >= 0.0f)
		{
			return true;
		}
	}
	if (inValue > 0.0f)
	{
		if (inValue - GetEpsilon() <= 0.0f)
		{
			return true;
		}
	}

	return false;
}

bool IsNear(const float a, const float b)
{
	if ((a > 0.0f && b > 0.0f) || (a < 0.0f && b < 0.0f))
	{
		if (IsNearZero(AbsoluteValue(a - b)))
			return true;
		else
			return false;		
	}
	else
	{
		if(IsNearZero(AbsoluteValue(a + b)))
			return true;
		else
			return false;
	}
}

float AbsoluteValue(float value)
{
	if (value < 0.0f)
	{
		return (value * -1.0f);
	}
	return value;
}

float GetMagnitude(float inputFloat)
{
	if(inputFloat < 0.f)
	{
		inputFloat *= -1.f;
	}
	return inputFloat;
}

float GetAbsoluteValue(float inputFloat)
{
	return GetMagnitude(inputFloat);
}

bool SolveQuadratic(Vector2* out, float a, float b, float c)
{
	if(a == 0)
	{
		return false;
	}

	float floatUnderSqrt = (b * b) - ( 4.f * a * c);
	
	if(floatUnderSqrt > 0.f)
	{
		float firstRoot = ((-1.f * b) - sqrtf(floatUnderSqrt)) / (2.f * a);

		float secondRoot = ((-1.f * b) + sqrtf(floatUnderSqrt)) / (2.f * a);

		if(firstRoot < secondRoot)
		{
			out->x = firstRoot;
			out->y = secondRoot;
		}

		else
		{
			out->x = secondRoot;
			out->y = firstRoot;
		}		

		return true;
	}

	return false;
}

bool GetIsPointInsideCube(const Vector3& pointToCheck, const Vector3& cubeCenterPosition, const Vector3& dimensions)
{
	bool isPointInside = false;
	float halfDimensionX = dimensions.x * 0.5f;
	float halfDimensionY = dimensions.y * 0.5f;
	float halfDimensionZ = dimensions.z * 0.5f;

	if(pointToCheck.x < cubeCenterPosition.x + halfDimensionX && pointToCheck.x > cubeCenterPosition.x - halfDimensionX
		&& pointToCheck.y < cubeCenterPosition.y + halfDimensionY && pointToCheck.y > cubeCenterPosition.y - halfDimensionY
		&& pointToCheck.z < cubeCenterPosition.z + halfDimensionZ && pointToCheck.z > cubeCenterPosition.z - halfDimensionZ)
	{
		isPointInside = true;
	}
	
	return isPointInside;
}

float Modulus(float dividend, float divisor)
{
	return fmodf((divisor + fmodf(dividend, divisor)), divisor);
}

float Modulus(Vector2 mod)
{
	return fmodf((mod.y + fmodf(mod.x, mod.y)), mod.y);
}

double Modulus(double dividend, double divisor)
{
	return fmodf((divisor + fmodf(dividend, divisor)), divisor);
}

int Modulus(int dividend, int divisor)
{
	return (divisor + (dividend % divisor)) % divisor;
}

Vector2 PolarDegreesToCartesian(float radius, float degrees)
{
	float x = radius * CosDegrees(degrees);
	float y = radius * SinDegrees(degrees);

	return Vector2(x, y);
}

Vector2 PolarRadiansToCartesian(float radius, float radians)
{
	float degrees = ConvertRadiansToDegrees(radians);
	return PolarDegreesToCartesian(radius, degrees);
}

Vector3 SphericalToCartesian(float radius, float rotation, float azimuth)
{
	float x = radius * CosDegrees(azimuth) * CosDegrees(rotation);
	float y = radius * SinDegrees(azimuth);
	float z = radius * CosDegrees(azimuth) * SinDegrees(rotation);

	return Vector3(x, y, z);
}

Vector3 SphericalToCartesian(Vector3 spherical)
{
	return SphericalToCartesian(spherical.x, spherical.y, spherical.z);
}

Vector3 CartesianToSpherical(Vector3 position)
{
	float radius = position.GetLength();
	float rotation = AtanfAsDegrees(position.y, position.x);
	float azimuth = AcosfAsDegrees(position.z, radius);

	return Vector3(radius, rotation, azimuth);
}

Vector2 CartesianToPolarDegrees(Vector2 position)
{
	float radius = position.GetLength();
	float rotation = AtanfAsDegrees(position.y, position.x);

	return Vector2(radius, rotation);
}
