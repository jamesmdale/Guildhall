#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Math\IntVector2.hpp"
#include <string>

const Vector2 Vector2::ZERO = Vector2(0.f, 0.f);
const Vector2 Vector2::ZEROTOONE = Vector2(0.f, 1.f);
const Vector2 Vector2::ONE = Vector2(1.f, 1.f);
const Vector2 Vector2::NEGATIVE_ONE = Vector2(-1.f, -1.f);

const Vector2 Vector2::DOWN = Vector2(0.f, -1.f);
const Vector2 Vector2::UP = Vector2(0.f, 1.f);;
const Vector2 Vector2::RIGHT = Vector2(1.f, 0.f);;
const Vector2 Vector2::LEFT = Vector2(-1.f, 0.f);;

//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )	
{
	x = copy.x;
	y = copy.y;
}


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
{
	x = initialX;
	y = initialY;
}

Vector2::Vector2(const IntVector2& vector)
{
	x = (float)vector.x;
	y = (float)vector.y;
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator + ( const Vector2& vecToAdd ) const
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y ); // #MP1Fixme
}

const Vector2 Vector2::operator*(const Vector2& vecToMultiply) const
{
	return Vector2(x * vecToMultiply.x, y * vecToMultiply.y);
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	return Vector2( x * uniformScale, y * uniformScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	return Vector2( x/inverseScale, y/inverseScale ); // #MP1Fixme
}

const Vector2 Vector2::operator/(const Vector2 & vecToDivide) const
{
	return Vector2(x/vecToDivide.x, y/vecToDivide.y);
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	x = x + vecToAdd.x; // #MP1Fixme
	y = y + vecToAdd.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	x = x - vecToSubtract.x; // #MP1Fixme
	y = y - vecToSubtract.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	x = x * uniformScale; // #MP1Fixme
	y = y * uniformScale; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	x = x / uniformDivisor; // #MP1Fixme
	y = y / uniformDivisor; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	x = copyFrom.x; // #MP1Fixme
	y = copyFrom.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	bool isEqual = false;

	if(x == compare.x && y == compare.y)
		isEqual = true;

	return isEqual;
}

//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	bool isNotEqual = true;

	if(x == compare.x && y == compare.y)
		isNotEqual = false;

	return isNotEqual;
}

Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	return Vector2( vecToScale.x * uniformScale, vecToScale.y * uniformScale); // #MP1Fixme
}


float Vector2::GetLength() const
{
	return sqrtf((x*x) + (y*y));
}

float Vector2::GetLengthSquared() const
{
	return ((x*x)+(y*y));
}

float Vector2::NormalizeAndGetLength()
{
	float vectorLength = this->GetLength(); //probably should be fixed or renamed to GetLengthAndNormalize

	if(vectorLength != 0.f)
	{
		this->x = x/vectorLength;
		this->y = y/vectorLength;
	}
	
	return vectorLength;
}

Vector2 Vector2::GetNormalized() const
{
	float vectorLength = this->GetLength();
	if(vectorLength != 0.f)
	{
		return Vector2((x/vectorLength), (y/vectorLength));
	}
	else
		return Vector2(0, 0);
}

Vector2 Vector2::MakeDirectionAtDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);

	return Vector2(cosf(radians), sinf(radians));
}

float Vector2::GetOrientationDegrees() const
{
	Vector2 origin = Vector2(0, 0);
	float degrees;
	
	degrees = AtanfAsDegrees(y, x);
	
	return degrees;
}

void Vector2::SetFromText(const char* text)
{
	std::string str(text);	
	int delimiterIndex = (int)str.find(',');

	std::string xVal = str.substr(0, delimiterIndex);
	std::string yVal = str.substr(delimiterIndex + 1);
	x = (float)atof(xVal.c_str());
	y = (float)atof(yVal.c_str());

	return;	
}

float Vector2::Dot(const Vector2& dotVector)
{
	return DotProduct(*this, dotVector);
}

void Vector2::Floor()
{
	x = floorf(x);
	y = floorf(y);
}

float GetDistance(const Vector2& a, const Vector2& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float returnFloat = sqrtf((dx * dx) + (dy * dy));
	return returnFloat;
}

float GetRadius(const Vector2& a)
{
	float radius = sqrtf((a.x * a.x) + (a.y * a.y));
	return radius;
}

float GetDistanceSquared(const Vector2& a, const Vector2& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;

	return ((dx * dx) + (dy * dy));
}

int MoveRegularPolygonX(float centerX, Vector2 movementVector)
{
	centerX += movementVector.x;

	return (int)centerX;
}
int MoveRegularPolygonY(float centerY, Vector2 movementVector)
{
	centerY += movementVector.y;

	return (int)centerY;
}

Vector2 GetMidpoint(const Vector2& a, const Vector2& b)
{
	return Vector2((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
}

const Vector2 GetProjectedVector(const Vector2& vectorToProject, const Vector2& projectOnto)
{
	return (DotProduct(vectorToProject, projectOnto.GetNormalized()) * projectOnto.GetNormalized());
}
const Vector2 GetTransformedIntoBasis (const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ)
{
	return Vector2(DotProduct(originalVector, newBasisI.GetNormalized()), DotProduct(originalVector, newBasisJ.GetNormalized()));
}
const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ )
{
	return Vector2((vectorInBasis.x * oldBasisI.x) + (vectorInBasis.y * oldBasisJ.x), (vectorInBasis.x * oldBasisI.y) +  (vectorInBasis.y * oldBasisJ.y));	
}
void DecomposeVectorIntoBasis( const Vector2& originalVector,	const Vector2& newBasisI, const Vector2& newBasisJ,	Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ )
{
	out_vectorAlongI = (DotProduct(originalVector, newBasisI.GetNormalized()) * newBasisI);
	out_vectorAlongJ = (DotProduct(originalVector, newBasisJ.GetNormalized()) * newBasisJ);
}

const Vector2 Reflect(const Vector2& originalVector2, const Vector2& bounceDirection)
{
	Vector2 projectedVector = GetProjectedVector(originalVector2, bounceDirection);
	return (originalVector2 - (2 * projectedVector));
}

const Vector2 Interpolate(const Vector2& start, const Vector2& end, float fractionTowardEnd)
{
	return Vector2(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd));
}

float GetLargerOfXY(const Vector2& vector)
{
	if(vector.y > vector.x)
	{
		return vector.y;
	}

	return vector.x;
}
