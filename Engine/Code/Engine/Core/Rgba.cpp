#include "engine\Core\Rgba.hpp"
#include "engine\Math\MathUtils.hpp"
#include <string>
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\Vector3.hpp"

const Rgba Rgba::RED = Rgba(255, 0, 0, 255);
const Rgba Rgba::ORANGE = Rgba(255, 140, 0, 255);
const Rgba Rgba::GREEN = Rgba(0, 255, 0, 255);
const Rgba Rgba::BLUE = Rgba(0, 0, 255, 255);
const Rgba Rgba::LIGHT_BLUE = Rgba(126, 221, 214, 255);
const Rgba Rgba::WHITE = Rgba(255, 255, 255, 255);
const Rgba Rgba::YELLOW = Rgba(255, 255, 0, 255);
const Rgba Rgba::PURPLE = Rgba(255, 0, 255, 255);
const Rgba Rgba::BLACK = Rgba(0, 0, 0, 255);
const Rgba Rgba::NEAR_BLACK = Rgba(3, 3, 3, 255);
const Rgba Rgba::CLEAR = Rgba(0, 0, 0, 0);
const Rgba Rgba::GRAY = Rgba(100, 100, 100, 255);
const Rgba Rgba::PINK = Rgba (255, 0, 255, 255);

//transparent colors
const Rgba Rgba::BLUE_TRANSPARENT = Rgba(0, 0, 255, 200);
const Rgba Rgba::LIGHT_ORANGE_TRANSPARENT = Rgba(255, 140, 0, 100);
const Rgba Rgba::LIGHT_RED_TRANSPARENT = Rgba(248, 124, 124, 200);
const Rgba Rgba::LIGHT_BLUE_TRANSPARENT = Rgba(124, 124, 248, 200);
const Rgba Rgba::LIGHT_GREEN_TRANSPARENT = Rgba(124, 248, 124, 200);
const Rgba Rgba::LIGHT_WHITE_TRANSPARENT = Rgba(255, 255, 255, 50);
const Rgba Rgba::LIGHT_PINK_TRANSPARENT = Rgba (255, 0, 255, 100);

const Rgba Rgba::NORMAL_MAP_FLAT = Rgba(127, 127, 255, 255);

Rgba::Rgba()//default constructs to opaque white (255, 255, 255, 255)
{
	r = 255;
	g = 255;
	b = 255;
	a = 255;
}
Rgba::Rgba(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

Rgba::Rgba(int redByte, int greenByte, int blueByte, int alphaByte)
{
	r = (unsigned char)redByte;
	g = (unsigned char)greenByte;
	b = (unsigned char)blueByte;
	a = (unsigned char)alphaByte;
}

Rgba::Rgba(Vector4 color)
{
	SetAsFloats(color.x, color.y, color.z, color.w);
}

Rgba::Rgba(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	SetAsFloats(normalizedRed, normalizedGreen, normalizedBlue, normalizedAlpha);
}

void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = (char)(normalizedRed * 255);
	g = (char)(normalizedGreen * 255);
	b = (char)(normalizedBlue * 255);
	a = (char)(normalizedAlpha * 255);
}

void Rgba::SetRedAsFloat(float normalizedRed)
{
	r = (char)(normalizedRed * 255);
}

void Rgba::SetGreenAsFloat(float normalizedGreen)
{
	g = (char)(normalizedGreen * 255);
}

void Rgba::SetBlueAsFloat(float normalizedBlue)
{
	b = (char)(normalizedBlue * 255);
}

void Rgba::SetAlphaAsFloat(float normalizedAlpha)
{
	a = (char)(normalizedAlpha * 255);
}

float Rgba::GetRedAsFloat()
{
	float divisor = 1.f / 255.f;

	float red = r * divisor;

	float redNormalized = ClampFloatZeroToOne(red);
	return redNormalized;
}

float Rgba::GetGreenAsFloat()
{
	float divisor = 1.f / 255.f;

	float green = g * divisor;

	float greenNormalized = ClampFloatZeroToOne(green);
	return greenNormalized;
}

float Rgba::GetBlueAsFloat()
{
	float divisor = 1.f / 255.f;

	float blue = a * divisor;

	float blueNormalized = ClampFloatZeroToOne(blue);
	return blueNormalized;
}

float Rgba::GetAlphaAsFloat()
{
	float divisor = 1.f / 255.f;

	float alpha = a * divisor;

	float alphaNormalized = ClampFloatZeroToOne(alpha);
	return alphaNormalized;
}

void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	float divisor = 1.f/255.f;
	out_normalizedRed *= divisor;
	out_normalizedGreen *= divisor;
	out_normalizedBlue *= divisor;
	out_normalizedAlpha *= divisor;

	out_normalizedRed = ClampFloatZeroToOne(out_normalizedRed);
	out_normalizedGreen = ClampFloatZeroToOne(out_normalizedGreen);
	out_normalizedBlue = ClampFloatZeroToOne(out_normalizedBlue);
	out_normalizedAlpha = ClampFloatZeroToOne(out_normalizedAlpha);
}

Vector4 Rgba::ConvertToVector4(const Rgba& color)
{
	float divisor = 1.f/255.f;
	Vector4 outVector;

	outVector.x = color.r * divisor;
	outVector.y = color.g * divisor;
	outVector.z = color.b * divisor;
	outVector.w = color.a * divisor;

	outVector.x = ClampFloatZeroToOne(outVector.x);
	outVector.y = ClampFloatZeroToOne(outVector.y);
	outVector.z = ClampFloatZeroToOne(outVector.z);
	outVector.w = ClampFloatZeroToOne(outVector.w);

	return outVector;
}

Vector3 Rgba::ConvertToVector3(const Rgba& color)
{
	float divisor = 1.f/255.f;
	Vector3 outVector;

	outVector.x = color.r * divisor;
	outVector.y = color.g * divisor;
	outVector.z = color.b * divisor;

	outVector.x = ClampFloatZeroToOne(outVector.x);
	outVector.y = ClampFloatZeroToOne(outVector.y);
	outVector.z = ClampFloatZeroToOne(outVector.z);

	return outVector;
}





void Rgba::ScaleRGB(float rgbScale) //scales (and clamps) rgb components but not A
{
	r *= (unsigned char)rgbScale;
	g *= (unsigned char)rgbScale;
	b *= (unsigned char)rgbScale;
	a *= (unsigned char)rgbScale;

	r = (unsigned char)ClampFloat(r, 0.f, 255.f);
	g = (unsigned char)ClampFloat(r, 0.f, 255.f);
	b = (unsigned char)ClampFloat(r, 0.f, 255.f);
	a = (unsigned char)ClampFloat(r, 0.f, 255.f);
}

void Rgba::ScaleAlpha(float alphaScale) //scales and clamps Alpha, RGB is untouched
{
	a *= (unsigned char)alphaScale;

	a = (unsigned char)ClampFloat(r, 0.f, 255.f);
}

void Rgba::SetFromText(const char* text)
{
	std::string str(text);	
	int delimiterIndexFirst = (int)str.find(',');
	int delimiterIndexSecond = (int)str.find(',', delimiterIndexFirst + 1);
	int delimiterIndexThird = (int)str.find(',', delimiterIndexSecond + 1);	

	std::string rVal = str.substr(0, delimiterIndexFirst);
	r = (unsigned char)atof(rVal.c_str());

	int lengthSecond = delimiterIndexSecond - (delimiterIndexFirst + 1);
	std::string gVal = str.substr(delimiterIndexFirst + 1, lengthSecond);
	g = (unsigned char)atof(gVal.c_str());

	if(delimiterIndexThird != -1)
	{
		int lengthThird = delimiterIndexThird - (delimiterIndexThird + 1);
		std::string bVal = str.substr(delimiterIndexSecond + 1, lengthThird);
		b  = (unsigned char)atof(bVal.c_str());

		std::string aVal = str.substr(delimiterIndexThird + 1);
		a  = (unsigned char)atof(aVal.c_str());
	}
	else
	{
		std::string bVal = str.substr(delimiterIndexSecond + 1);
		b  = (unsigned char)atof(bVal.c_str());

		a = (unsigned char)255.f;
	}

	return;	
}

const Rgba Interpolate( Rgba& start, Rgba& end, float fractionTowardEnd)
{
	unsigned char lerpedR = (unsigned char)Interpolate(start.r, end.r, fractionTowardEnd);
	unsigned char lerpedG = (unsigned char)Interpolate(start.g, end.g, fractionTowardEnd);
	unsigned char lerpedB = (unsigned char)Interpolate(start.b, end.b, fractionTowardEnd);
	unsigned char lerpedA = (unsigned char)Interpolate(start.a, end.a, fractionTowardEnd);

	return Rgba(lerpedR, lerpedG, lerpedB, lerpedA);
}

bool Rgba::operator==( const Rgba& compare ) const
{
	bool isEqual = false;

	if(r == compare.r && g == compare.g && b == compare.b && a == compare.a)
	{
		isEqual = true;
	}

	return isEqual;
}

bool Rgba::operator!=(const Rgba & compare) const
{
	bool isNotEqual = true;

	if(r == compare.r && g == compare.g && b == compare.b && a == compare.a)
	{
		isNotEqual = false;
	}

	return isNotEqual;
}

