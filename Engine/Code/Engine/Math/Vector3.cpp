#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <string>

const Vector3 Vector3::ZERO = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::ONE = Vector3(1.f, 1.f, 1.f);
const Vector3 Vector3::RIGHT = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::UP = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::FORWARD = Vector3(0.f, 0.f, 1.f);

const Vector3 Vector3::DEFAULT_LIGHT_ATTENUATION = Vector3(0, 1.f, 0.75f);

const Vector3 Vector3::GRAVITY_FORCE = Vector3(0.f, -9.8f, 0.f);

Vector3::Vector3()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
}

//-----------------------------------------------------------------------------------------------
Vector3::Vector3( const Vector3& copy )	
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}


//-----------------------------------------------------------------------------------------------
Vector3::Vector3( float initialX, float initialY, float initialZ )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3::Vector3(const float& uniformSize)
{
	x = uniformSize;
	y = uniformSize;
	z = uniformSize;
}

Vector3::Vector3(const IntVector3& vector)
{
	x = (float) vector.x;
	y = (float) vector.y;
	z = (float) vector.z;
}

Vector3::Vector3(const Vector4& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}

Vector3::Vector3(const Vector2 & vector)
{
	x = vector.x;
	y = vector.y;
	z = 0.f;
}

//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator + ( const Vector3& vecToAdd ) const
{
	return Vector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator-( const Vector3& vecToSubtract ) const
{
	return Vector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator*( float uniformScale ) const
{
	return Vector3( x * uniformScale, y * uniformScale, z * uniformScale ); // #MP1Fixme
}

const Vector3 Vector3::operator*(const Vector3& uniformScale) const
{
	return Vector3(x * uniformScale.x, y * uniformScale.y, z * uniformScale.z);
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator/( float inverseScale ) const
{
	return Vector3( x/inverseScale, y/inverseScale, z/inverseScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator+=( const Vector3& vecToAdd )
{
	x = x + vecToAdd.x; 
	y = y + vecToAdd.y; 
	z = z + vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator-=( const Vector3& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
	z = z - vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale; 
	z = z * uniformScale;
}

void Vector3::operator*=( const Vector3& uniformScale )
{
	x = x * uniformScale.x;
	y = y * uniformScale.y; 
	z = z * uniformScale.z;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator/=( const float uniformDivisor )
{
	x = x / uniformDivisor; 
	y = y / uniformDivisor; 
	z = z / uniformDivisor; 
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator=( const Vector3& copyFrom )
{
	x = copyFrom.x; 
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
bool Vector3::operator==( const Vector3& compare ) const
{
	bool isEqual = false;

	if(x == compare.x && y == compare.y && z == compare.z)
		isEqual = true;

	return isEqual;
}

//-----------------------------------------------------------------------------------------------
bool Vector3::operator!=( const Vector3& compare ) const
{
	bool isNotEqual = true;

	if(x == compare.x && y == compare.y && z == compare.z)
		isNotEqual = false;

	return isNotEqual;
}

Vector3 operator*( float uniformScale, const Vector3& vecToScale )
{
	return Vector3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}

void Vector3::SetFromText(const char* text)
{
	std::string str(text);	

	//get x val
	int delimiterIndex = (int)str.find(',');
	std::string xVal = str.substr(0, delimiterIndex);

	//get y val
	str = str.substr(delimiterIndex + 1);
	delimiterIndex = (int)str.find(',');
	std::string yVal = str.substr(0, delimiterIndex);

	//get z val
	str = str.substr(delimiterIndex + 1);
	delimiterIndex = (int)str.find(',');
	std::string zVal = str.substr(0, delimiterIndex);

	x = (float)atof(xVal.c_str());
	y = (float)atof(yVal.c_str());
	z = (float)atof(zVal.c_str());

	return;	
}

float Vector3::GetLength() const
{
	return sqrtf((x*x) + (y*y) + (z*z));
}

float Vector3::Dot(const Vector3 & dotVector)
{
	return DotProduct(*this, dotVector);
}

float Vector3::GetLengthSquared() const
{
	return ((x*x)+(y*y)+(z*z));
}

float Vector3::NormalizeAndGetLength()
{
	float vectorLength = this->GetLength(); //probably should be fixed or renamed to GetLengthAndNormalize

	if(vectorLength != 0.f)
	{
		this->x = x/vectorLength;
		this->y = y/vectorLength;
		this->z = z/vectorLength;
	}

	return vectorLength;
}

Vector3 Vector3::GetNormalized() const
{
	float vectorLength = this->GetLength();
	if(vectorLength != 0.f)
	{
		return Vector3((x/vectorLength), (y/vectorLength), (z/vectorLength));
	}
	else
		return Vector3(0, 0, 0);
}

void Vector3::Normalize()
{
	float vectorLength = this->GetLength(); //probably should be fixed or renamed to GetLengthAndNormalize

	if(vectorLength != 0.f)
	{
		this->x = x/vectorLength;
		this->y = y/vectorLength;
		this->z = z/vectorLength;
	}
}

//default is -0.01 - 0.01
bool Vector3::CompareZero(float inclusiveMin, float inclusiveMax)
{
	bool isNearZero = false;
	
	if((x >= inclusiveMin && x <= inclusiveMax) && (y >= inclusiveMin && y <= inclusiveMax), (z >= inclusiveMin && z <= inclusiveMax) )
	{
		isNearZero = true;
	}

	return isNearZero;
}

TODO("DirectionAtDegrees")
//Vector2 Vector2::MakeDirectionAtDegrees(float degrees)
//{
//	float radians = ConvertDegreesToRadians(degrees);
//
//	return Vector2(cosf(radians), sinf(radians));
//}

TODO("DirectionAtDegrees")
//float Vector2::GetOrientationDegrees() const
//{
//	Vector2 origin = Vector2(0, 0);
//	float degrees;
//
//	degrees = AtanfAsDegrees(y, x);
//
//	return degrees;
//}

TODO("SETFROMTEXT")
//void Vector2::SetFromText(const char* text)
//{
//	std::string str(text);	
//	int delimiterIndex = (int)str.find(',');
//
//	std::string xVal = str.substr(0, delimiterIndex);
//	std::string yVal = str.substr(delimiterIndex + 1);
//	std::string zVal = str.substr(delim
//	x = (float)atof(xVal.c_str());
//	y = (float)atof(yVal.c_str());
//	z = (float)atof(zVal.c_str());
//
//	return;	
//}

float GetDistance(const Vector3& a, const Vector3& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	float returnFloat = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
	return returnFloat;
}

float GetRadius(const Vector3& a)
{
	float radius = sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
	return radius;
}

float GetDistanceSquared(const Vector3& a, const Vector3& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return ((dx * dx) + (dy * dy) + (dz * dz));
}

Vector3 GetMin(const Vector3 & a, const Vector3 & b)
{
	return Vector3(GetMinFloat(a.x, b.x),
		GetMinFloat(a.y, b.y),
		GetMinFloat(a.z, b.z));
}

int MoveRegularPolygonX(float centerX, Vector3 movementVector)
{
	centerX += movementVector.x;

	return (int)centerX;
}
int MoveRegularPolygonY(float centerY, Vector3 movementVector)
{
	centerY += movementVector.y;

	return (int)centerY;
}

Vector3 AbsoluteValue(const Vector3& initialValue)
{
	return Vector3(AbsoluteValue(initialValue.x), AbsoluteValue(initialValue.y), AbsoluteValue(initialValue.z));
}


Vector3 GetMidpoint(const Vector3& a, const Vector3& b)
{
	return Vector3((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f);
}

const Vector3 Interpolate(const Vector3& start, const Vector3& end, float fractionTowardEnd)
{
	return Vector3(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd), Interpolate(start.z, end.z, fractionTowardEnd));
}

const Vector3 SphericalInterpolate(const Vector3 & start, const Vector3 & end, float fractionTowardEnd)
{
	float startLength = start.GetLength();
	float endLength = end.GetLength();

	float length = Interpolate(startLength, endLength, fractionTowardEnd);
	
	Vector3 unit = SphericalInterpolateUnit(start/startLength, end/endLength, fractionTowardEnd);
	return length * unit;
}

const Vector3 SphericalInterpolateUnit(const Vector3 & start, const Vector3 & end, float fractionTowardEnd)
{
	float cosAngle = ClampFloat(DotProduct(start, end), -1.0f, 1.0f);
	float angle = acosf(cosAngle);
	if (angle < GetEpsilon())
	{
		return Interpolate(start, end, fractionTowardEnd);
	}
	else
	{
		float positiveNum = sinf(fractionTowardEnd * angle);
		float negativeNum = sinf((1.0f - fractionTowardEnd) * angle);
		float denominator = sinf(angle);

		return ((negativeNum / denominator) * start) + ((positiveNum/denominator) * end);
	}

}

TODO("Projections, transformations, dots, decompositions, reflections, and interpolate")
//
//const Vector2 GetProjectedVector(const Vector3& vectorToProject, const Vector3& projectOnto)
//{
//	return (DotProduct(vectorToProject, projectOnto.GetNormalized()) * projectOnto.GetNormalized());
//}
//const Vector2 GetTransformedIntoBasis (const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ)
//{
//	return Vector2(DotProduct(originalVector, newBasisI.GetNormalized()), DotProduct(originalVector, newBasisJ.GetNormalized()));
//}
//const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ )
//{
//	return Vector2((vectorInBasis.x * oldBasisI.x) + (vectorInBasis.y * oldBasisJ.x), (vectorInBasis.x * oldBasisI.y) +  (vectorInBasis.y * oldBasisJ.y));	
//}
//void DecomposeVectorIntoBasis( const Vector2& originalVector,	const Vector2& newBasisI, const Vector2& newBasisJ,	Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ )
//{
//	out_vectorAlongI = (DotProduct(originalVector, newBasisI.GetNormalized()) * newBasisI);
//	out_vectorAlongJ = (DotProduct(originalVector, newBasisJ.GetNormalized()) * newBasisJ);
//}
//
//const Vector2 Reflect(const Vector2& originalVector2, const Vector2& bounceDirection)
//{
//	Vector2 projectedVector = GetProjectedVector(originalVector2, bounceDirection);
//	return (originalVector2 - (2 * projectedVector));
//}
//
//const Vector2 Interpolate(const Vector2& start, const Vector2& end, float fractionTowardEnd)
//{
//
//	return Vector2(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd));
//}
