#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Engine\Core\Rgba.hpp"
#include "Engine\Math\Vector2.hpp"


const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::vector<std::string> SplitStringOnCharacter(const std::string inputString, const char splitChar);
std::string SplitStringOnFirstWord(const std::string& inputString);
size_t GetStringSize(const std::string& inputString);

//string checks
bool IsStringNullOrEmpty(const char* inputString);
bool IsStringNullOrEmpty(const std::string& inputString);
bool DoesStringContainCharacter(const std::string& inputString, const char character);
bool CompareStringAlphabeticalLessThan(const std::string& stringOne, const std::string& stringTwo);
bool StringCompareExact(const std::string& stringOne, const std::string& stringTwo);

//removal
void RemoveEmptyStrings(std::vector<std::string>& outStrings);
void ReplaceCharacterOccurances(std::string & outString, const char characterToReplace, const char replacementCharacter);
void RemoveMatchingStrings(std::vector<std::string>& outStrings, const std::string& matchingString);
void RemoveStringsStartingWithString(std::vector<std::string>& outStrings, const std::string& startingString );

//conversions from string
int ConvertStringToInt(const std::string& convertString);
float ConvertStringToFloat(const std::string& convertString);
Rgba ConvertStringToRGBA(const std::string& convertString);
bool ConvertStringToBool(const std::string&convertString);
Vector2 ConvertStringToVector2(const std::string& convertString);
void ToLower(std::string& convertString);
std::string ToLowerAsNew(const std::string & converString);

//trim
std::string TrimStringToDecimalPlace(std::string stringToTrim); //this is janky





