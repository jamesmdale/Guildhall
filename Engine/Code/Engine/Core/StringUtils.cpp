#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <sstream>
#include "Engine\Core\EngineCommon.hpp"
#include <string.h>

//  =============================================================================
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//  =============================================================================
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//  =============================================================================
const std::vector<std::string> SplitStringOnCharacter(const std::string str, const char splitChar)
{
	std::stringstream stream(str);
	std::vector<std::string> splitString;
	while (stream.good()) 
	{
		std::string substr;
		std::getline( stream, substr, splitChar );
		splitString.push_back( substr );
	}

	return splitString;
}

//  =============================================================================
std::string SplitStringOnFirstWord(const std::string& inputString)
{
	std::string returnString = "";
	
	int index = (int)inputString.find_first_of(' ');
	returnString = inputString.substr(0, index);

	return returnString;
}

size_t GetStringSize(const std::string & inputString)
{
	return inputString.size();
}

bool IsStringNullOrEmpty(const char* inputString)
{
	std::string input = std::string(inputString);
	return IsStringNullOrEmpty(input);
}

//  =============================================================================
bool IsStringNullOrEmpty(const std::string & inputString)
{
	if(inputString.empty() || inputString == "")
		return true;

	return false;
}

//  =============================================================================
bool DoesStringContainCharacter(const std::string& inputString, const char character)
{
	//if string contains the character, it will return a position. Else, return -1
	return !(inputString.find(character) == std::string::npos);
}

//  =========================================================================================
bool CompareStringAlphabeticalLessThan(const std::string& stringOne, const std::string& stringTwo)
{
	std::string stringOneLower = stringOne;
	std::string stringTwoLower = stringTwo;

	ToLower(stringOneLower);
	ToLower(stringTwoLower);

	int compare = stringOne.compare(stringTwo);

	if (compare > 0)
	{
		return true;
	}

	return false;
}

bool StringCompareExact(const std::string& stringOne, const std::string& stringTwo)
{
	//first compare length (easiest out)
	if(stringOne.length() != stringTwo.length())
		return false;

	//compare each character
	for (int characterIndex = 0; characterIndex < (int)stringOne.length(); ++characterIndex)
	{
		if(stringOne[characterIndex] != stringTwo[characterIndex])
			return false;
	}

	return true;
}

//  =============================================================================
void RemoveEmptyStrings(std::vector<std::string>& outStrings)
{
	RemoveMatchingStrings(outStrings, "");
}

//  =============================================================================
void ReplaceCharacterOccurances(std::string& outString, const char characterToReplace, const char replacementCharacter)
{
	for (int stringIndex = 0; stringIndex < (int)outString.size(); stringIndex++)
	{
		if (outString[stringIndex] == characterToReplace)
		{
			outString[stringIndex] = replacementCharacter;
		}
	}
}

//  =============================================================================
void RemoveMatchingStrings(std::vector<std::string>& outStrings, const std::string& matchingString)
{
	for(int stringIndex = 0; stringIndex < (int)outStrings.size(); stringIndex++)
	{
		if(outStrings[stringIndex] == matchingString)
		{
			outStrings.erase(outStrings.begin() + stringIndex);
			stringIndex--;
		}				
	}
}

//  =============================================================================
void RemoveStringsStartingWithString(std::vector<std::string>& outStrings, const std::string& startingString )
{
	size_t stringLength = startingString.length();

	for(int stringIndex = 0; stringIndex < (int)outStrings.size(); stringIndex++)
	{
		std::string out = outStrings[stringIndex].substr(0, 1);
		if(outStrings[stringIndex].substr(0, stringLength) == startingString)
		{
			outStrings.erase(outStrings.begin() + stringIndex);
			stringIndex--;
		}					
	}
}

//  =============================================================================
int ConvertStringToInt(const std::string& convertString)
{
	if (convertString != "")
	{
		int returnVal = stoi(convertString);	
		return returnVal;		
	}	

	return INT_MAX;
}

//  =============================================================================
float ConvertStringToFloat(const std::string& convertString)
{
	float returnVal = stof(convertString);
	if(returnVal != NULL)
	{
		return returnVal;
	}

	return NULL;
}

//  =============================================================================
bool ConvertStringToBool(const std::string& convertString)
{
	if(convertString != "")
	{
		if(convertString == "1" || convertString == "true")
		{
			return true;
		}		

		if(convertString == "0" || convertString == "false")
		{
			return false;
		}
	}

	return NULL;
}

//  =============================================================================
Vector2 ConvertStringToVector2(const std::string & convertString)
{
	std::vector<std::string> floats = SplitStringOnCharacter(convertString, ',');
	
	GUARANTEE_OR_DIE((int)floats.size() == 2, "IMPROPER CONVERSION OF STRING TO VECTOR2");

	float x = ConvertStringToFloat(floats[0]);
	float y = ConvertStringToFloat(floats[1]);

	return Vector2(x, y);
}

//  =============================================================================
Rgba ConvertStringToRGBA(const std::string& convertString)
{
	std::string str(convertString);	
	int delimiterIndexFirst = (int)str.find(',');
	int delimiterIndexSecond = (int)str.find(',', delimiterIndexFirst + 1);
	int delimiterIndexThird = (int)str.find(',', delimiterIndexSecond + 1);	
	
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	std::string rVal = str.substr(0, delimiterIndexFirst);
	int convertedInt = ConvertStringToInt(rVal);
	if(convertedInt == NULL || convertedInt > 255 || convertedInt < 0)
	{
		throw std::invalid_argument("INVALID: Incorrect value inputted for RGBA");
	}
	else
	{
		r = (unsigned char)convertedInt;
	}

	int lengthSecond = delimiterIndexSecond - (delimiterIndexFirst + 1);
	std::string gVal = str.substr(delimiterIndexFirst + 1, lengthSecond);
	convertedInt = ConvertStringToInt(gVal);
	if(convertedInt == NULL || convertedInt > 255 || convertedInt < 0)
	{
		throw std::invalid_argument("INVALID: Incorrect value inputted for RGBA");
	}
	else
	{
		g = (unsigned char)convertedInt;
	}

	if(delimiterIndexThird != -1)
	{
		int lengthThird = delimiterIndexThird - (delimiterIndexThird + 1);
		std::string bVal = str.substr(delimiterIndexSecond + 1, lengthThird);
		convertedInt = ConvertStringToInt(bVal);
		if(convertedInt == NULL || convertedInt > 255 || convertedInt < 0)
		{
			throw std::invalid_argument("INVALID: Incorrect value inputted for RGBA");
		}
		else
		{
			b = (unsigned char)convertedInt;
		}

		std::string aVal = str.substr(delimiterIndexThird + 1);
		convertedInt = ConvertStringToInt(aVal);
		if(convertedInt == NULL || convertedInt > 255 || convertedInt < 0)
		{
			throw std::invalid_argument("INVALID: Incorrect value inputted for RGBA");
		}
		else
		{
			a = (unsigned char)convertedInt;
		}
	}
	else
	{
		std::string bVal = str.substr(delimiterIndexSecond + 1);
		convertedInt = ConvertStringToInt(bVal);
		if(convertedInt == NULL || convertedInt > 255 || convertedInt < 0)
		{
			throw std::invalid_argument("INVALID: Incorrect value inputted for RGBA");
		}
		else
		{
			b = (unsigned char)convertedInt;
		}

		a = (unsigned char)255.f;
	}

	return Rgba(r,g,b,a);	
}

//  =============================================================================
void ToLower(std::string& convertString)
{
	for(int charIndex = 0; charIndex < (int)convertString.size(); charIndex++)
	{
		if(convertString[charIndex] >= 'A' && convertString[charIndex] <= 'Z')
		{
			convertString[charIndex] = convertString[charIndex] + 32;
		}
	}
}

//  =============================================================================
std::string ToLowerAsNew(const std::string& converString)
{
	std::string convertedString = converString;

	for(int charIndex = 0; charIndex < (int)convertedString.size(); charIndex++)
	{
		if(convertedString[charIndex] >= 'A' && convertedString[charIndex] <= 'Z')
		{
			convertedString[charIndex] = convertedString[charIndex] + 32;
		}
	}

	return convertedString;
}

//  =============================================================================
std::string TrimStringToDecimalPlace(std::string stringToTrim)
{
	TODO("This is a janky way of doing this.");
	for(int positionIndex = (int)stringToTrim.size(); positionIndex > 0; positionIndex--)
	{
		if(stringToTrim[positionIndex] != 46)
		{
			stringToTrim.pop_back();
		}		
		else
		{
			stringToTrim.pop_back();
			return stringToTrim;
		}
	}

	return stringToTrim;
}

//  =============================================================================
//void ToLower(std::string convertString)
//{
//	for(int charIndex = 0; charIndex < (int)convertString.size(); charIndex++)
//	{
//		if(convertString[charIndex] >= 'A' && convertString[charIndex] <= 'Z')
//		{
//			convertString[charIndex] = convertString[charIndex] + 32;
//		}
//	}
//}

//
//const std::vector<std::string> SplitStringOnDelimiter(const std::string str, const std::string delimiter)
//{
//	std::vector<std::string> splitString;
//
//	size_t pos = 0;
//	std::string token;
//	
//	while ((pos = str.find(delimiter)) != std::string::npos) {
//		token = str.substr(0, pos);
//		splitString.push_back(token);
//		str.erase((size_t)0, pos + delimiter.length());
//	}
//}





