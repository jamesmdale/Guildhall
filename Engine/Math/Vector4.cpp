#include "Engine\Math\Vector4.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <string>
#include "Engine\Core\StringUtils.hpp"

const Vector4 Vector4::IDENTITY_POINT_TRANSLATION = Vector4(0.f, 0.f, 0.f, 1.f);
const Vector4 Vector4::IDENTITY_VECTOR_TRANSLATION = Vector4(0.f, 0.f, 0.f, 0.f);

const Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::ONE = Vector4(1.f, 1.f, 1.f, 1.f);

Vector4::Vector4() {
	x = 0.f;
	y = 0.f;
	z = 0.f;
	w = 0.f;
}	

//-----------------------------------------------------------------------------------------------
Vector4::Vector4( const Vector4& copy )	
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
}

//-----------------------------------------------------------------------------------------------
Vector4::Vector4( float initialX, float initialY, float initialZ, float initialW )
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}

Vector4::Vector4( Vector3 vec, float appendedW)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = appendedW;
}

Vector4::Vector4(Rgba color)
{
	x = color.r;
	y = color.g;
	z = color.b;
	w = color.a;
}

void Vector4::SetFromText(const char* text)
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

	//get w val
	str = str.substr(delimiterIndex + 1);
	delimiterIndex = (int)str.find(',');
	std::string wVal = str.substr(0, delimiterIndex);


	x = (float)atof(xVal.c_str());
	y = (float)atof(yVal.c_str());
	z = (float)atof(zVal.c_str());
	w = (float)atof(wVal.c_str());

	return;	
}

float Vector4::Dot(const Vector4& dotVector)
{
	return DotProduct(*this, dotVector);
}

float Vector4::GetLength() const
{
	return sqrtf((x*x) + (y*y) + (z*z) + (w*w));
}

float Vector4::NormalizeAndGetLength()
{
	float vectorLength = this->GetLength(); //probably should be fixed or renamed to GetLengthAndNormalize

	if(vectorLength != 0.f)
	{
		this->x = x/vectorLength;
		this->y = y/vectorLength;
		this->z = z/vectorLength;
		this->w = w/vectorLength;
	}

	return vectorLength;
}

Vector4 Vector4::GetNormalized() const
{
	float vectorLength = this->GetLength();
	if(vectorLength != 0.f)
	{
		return Vector4((x/vectorLength), (y/vectorLength), (z/vectorLength), (w/vectorLength));
	}
	else
		return Vector4(0, 0, 0, 0);
}

void Vector4::Normalize()
{
	float vectorLength = this->GetLength(); //probably should be fixed or renamed to GetLengthAndNormalize

	if(vectorLength != 0.f)
	{
		this->x = x/vectorLength;
		this->y = y/vectorLength;
		this->z = z/vectorLength;
		this->w = w/vectorLength;
	}
}

const Vector4 Interpolate(const Vector4 & start, const Vector4 & end, float fractionTowardEnd)
{
	return Vector4(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd), Interpolate(start.z, end.z, fractionTowardEnd), Interpolate(start.w, end.w, fractionTowardEnd));	
}
