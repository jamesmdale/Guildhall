#pragma  once

class Vector4;
class Vector3;
class Rgba
{
public:
	Rgba(); //default constructs to opaque white (255, 255, 255, 255)

	explicit Rgba(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
	explicit Rgba(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha);
	explicit Rgba(int redByte, int greenByte, int blueByte, int alphaByte);
	explicit Rgba(Vector4 color);
	bool operator==( const Rgba& compare ) const;	
	bool operator!=(const Rgba& compare) const;

	//setters
	void SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
	void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f);
	void SetRedAsFloat(float normalizedRed);
	void SetGreenAsFloat(float normalizedGreen);
	void SetBlueAsFloat(float normalizedBlue);
	void SetAlphaAsFloat(float normalizedAlpha);

	//getters
	float GetRedAsFloat();
	float GetGreenAsFloat();
	float GetBlueAsFloat();
	float GetAlphaAsFloat();
	void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;

	//conversions
	static Vector4 ConvertToVector4(const Rgba& color);
	static Vector3 ConvertToVector3(const Rgba& color);
	void ScaleRGB(float rgbScale); //scales (and clamps) rgb components but not A
	void ScaleAlpha(float alphaScale); //scales and clamps Alpha, RGB is untouched
	void SetFromText(const char* text);
public:
	unsigned char r; //red byte, 0-255
	unsigned char g; //green byte, 0-255
	unsigned char b; //blue byte, 0-255
	unsigned char a; //alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaque	

	static const Rgba RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba WHITE;
	static const Rgba YELLOW;
	static const Rgba PURPLE;
	static const Rgba ORANGE;
	static const Rgba BLACK;
	static const Rgba NEAR_BLACK;
	static const Rgba CLEAR;
	static const Rgba GRAY;
	static const Rgba LIGHT_BLUE;
	static const Rgba LIGHT_BLUE_TRANSPARENT;
	static const Rgba PINK;
	static const Rgba BLUE_TRANSPARENT;
	static const Rgba LIGHT_PINK_TRANSPARENT;
	static const Rgba LIGHT_WHITE_TRANSPARENT;
	static const Rgba LIGHT_GREEN_TRANSPARENT;
	static const Rgba LIGHT_RED_TRANSPARENT;
	static const Rgba LIGHT_ORANGE_TRANSPARENT;
	static const Rgba NORMAL_MAP_FLAT;

};


const Rgba Interpolate( Rgba& start, Rgba& end, float fractionTowardEnd);


