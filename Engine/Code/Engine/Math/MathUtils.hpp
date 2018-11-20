#pragma once
#include <stdint.h>
//#include "Engine\Core\EngineCommon.hpp"

class Vector2;
class IntVector2;
class Vector3;
class Vector4;
class Matrix44;

//misc
int RoundToNearestInt( float inValue );
float RoundDownToDecimalPlace(float inValue, int decimalPlace); //100 would be 0.00
float CalculateXPoint(float centerX, float radius, float radians);
float CalculateYPoint(float centerY, float radius, float radians);
float GetMinFloat(float a, float b);
int GetMinInt(int a, int b);
int RotateRegularPolygon(float currentDegrees, float rotationDegrees);
float GetPi();
float GetEpsilon();
float GetInfinity();
bool IsNearZero(float inValue);
bool IsNear(const float a, const float b);
float AbsoluteValue(float value);
float GetMagnitude(float inputFloat);
float GetAbsoluteValue(float inputFloat);
bool SolveQuadratic(Vector2* out, float a, float b, float c); //Returns true if roots were found, false otherwise
bool GetIsPointInsideCube(const Vector3& pointToCheck, const Vector3& cubeCenterPosition, const Vector3& dimensions);

float Modulus(float dividend, float divisor);
float Modulus(Vector2 mod);
double Modulus(double dividend, double divisor);
int Modulus(int dividend, int divisor);

//trig functions
float CosDegrees(float degrees);
float TanDegrees(float degrees);
float SinDegrees(float degrees);
float AtanfAsDegrees(float y, float x);
float AcosfAsDegrees(float x, float radius);
float AsinfAsDegrees(float y, float radius);

//random generation functions
bool CheckRandomChance( float chanceForSuccess );
float GenerateRandomFloatInRangeLessThanOne(float min);
float GetRandomFloatZeroToOne();
float GetRandomFloatZeroOrOne();
int GetRandomIntZeroOrOne();
float GetRandomFloatInRange(float min, float max);
int GetRandomIntLessThan(int maxNotInclusive);
unsigned int GetRandomUintLessThan(unsigned int maxNotInclusive);
int GetRandomIntInRange(int min, int max);
unsigned int GetRandomUintInRange(unsigned int min, unsigned int max);
Vector3 GetRandomVector3InSphere(float positionMin, float positionMax);

//clamp functions
float ClampFloat(float inValue, float minInclusiveValue, float maxInclusiveValue);
double ClampDouble(double inValue, double minInclusiveValue, double maxInclusiveValue);
Vector3 ClampVector3Uniform(Vector3 inValue, Vector3 minInclusiveValue, Vector3 maxInclusiveValue);
Vector4 ClampVector4Uniform(Vector4 inValue, Vector4 minInclusiveValue, Vector4 maxInclusiveValue);
float ClampFloatZeroToOne(float inValue);
float ClampFloatNegativeOneToOne( float inValue );
int ClampInt(int inValue, int minInclusiveValue, int maxInclusiveValue);
float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float GetFractionInRange( float inValue, float rangeStart, float rangeEnd );

//rotation and dot product
float TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees );
float GetAngularDisplacement( float startDegrees, float endDegrees );
float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);
float DotProduct(const Vector4& a, const Vector4& b);
Vector3 CrossProduct(const Vector3 & a, const Vector3 & b);

//bit functions
bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );
void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );
void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );
void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );

//conversions
IntVector2 ConvertVector2ToIntVector2(Vector2 vectorToConvert);
Vector3 ConvertVector2ToVector3(Vector2 vectorToConvert);
Vector2 PolarDegreesToCartesian(float radius, float degrees);
Vector2 PolarRadiansToCartesian(float radius, float radians);
Vector3 SphericalToCartesian(float radius, float rotation, float azimuth);
Vector3 SphericalToCartesian(Vector3 spherical);
Vector3 CartesianToSpherical(Vector3 position);
Vector2 CartesianToPolarDegrees(Vector2 position);
float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);

//easing functions
float SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float SmoothStop2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float SmoothStop3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float SmoothStop4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float SmoothStep3( float t ); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)
float SmoothStartArch3(float t);
float GLSLSmoothStep(float edge0, float edge1, float x);
float Crossfade(float a, float b, float t);

//cyclic math
bool CyclicLessThan(uint16_t a, uint16_t b);
bool CyclicGreaterThan(uint16_t a, uint16_t b);

//template <typename T>
//bool CyclicLessThan(const T& a, const T& b)
//{
//	static T const HALF_MAX = (~(T)0) >> 1;
//	const T diff = b - a;
//	return (diff <= HALF_MAX);
//}

//interpolate
const int Interpolate(const int& start, const int& end, float fractionTowardEnd);
const unsigned char Interpolate(const unsigned char& start, const unsigned char& end, float fractionTowardEnd);
const float Interpolate(const float& start, const float& end, float fractionTowardEnd);
