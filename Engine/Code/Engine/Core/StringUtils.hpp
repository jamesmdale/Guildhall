#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Engine\Core\Rgba.hpp"


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::vector<std::string> SplitStringOnCharacter(const std::string inputString, const char splitChar);
std::string SplitStringOnFirstWord(const std::string& inputString);

//removal
void RemoveEmptyStrings(std::vector<std::string>& outStrings);
void RemoveMatchingStrings(std::vector<std::string>& outStrings, const std::string& matchingString);
void RemoveStringsStartingWithString(std::vector<std::string>& outStrings, const std::string& startingString );

//conversions
int ConvertStringToInt(std::string convertString);
float ConvertStringToFloat(std::string convertString);
Rgba ConvertStringToRGBA(std::string convertString);
bool ConvertStringToBool(std::string convertString);
void ToLower(std::string& convertString);
std::string ToLowerAsNew(const std::string & converString);

//trim
std::string TrimStringToDecimalPlace(std::string stringToTrim); //this is janky





