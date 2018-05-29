#pragma once
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\Core\StringUtils.hpp"


int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue )
{
	int parsedElement = defaultValue;
	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);
	if(elementText != "")
	{
		parsedElement = atoi(elementText.c_str());
	}

	return parsedElement;
}

char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue )
{
	char parsedElement = defaultValue;
	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);
	
	if(elementText != "")
	{
		parsedElement = elementText[0];
	}

	return parsedElement;
}

bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue )
{
	bool parsedElement = defaultValue;
	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);
	
	if(elementText != "")
	{
		if(elementText == "1" || elementText == "true")
		{
			parsedElement = true;
		}		

		if(elementText == "0" || elementText == "false")
		{
			parsedElement = false;
		}
	}

	return parsedElement;
}

float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue )
{
	float parsedElement = defaultValue;
	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement = (float)atof(elementText.c_str());
	}

	return parsedElement;
}

Rgba ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba& defaultValue )
{
	Rgba parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

Vector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector2& defaultValue )
{
	Vector2 parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

AABB2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const AABB2& defaultValue )
{
	AABB2 parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultValue )
{
	IntRange parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	FloatRange parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

IntVector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue )
{
	IntVector2 parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);
	if(elementText != "")
	{
		parsedElement.SetFromText(elementText.c_str());
	}	

	return parsedElement;
}

std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue )
{
	std::string parsedElement = defaultValue;

	const char* elementText = element.Attribute(attributeName);
	if(elementText != NULL)
	{
		parsedElement = elementText;
	}
	return parsedElement;
}

std::vector<int> ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::vector<int>& defaultValue )
{
	std::vector<int> parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);
	
	if(elementText != "")
	{
		std::vector<std::string> parsedStrings = SplitStringOnCharacter(elementText, ',');
		for(size_t stringIndex = 0; stringIndex < parsedStrings.size(); stringIndex++)
		{
			parsedElement.push_back(atoi(parsedStrings[stringIndex].c_str()));
		}		
	}	

	return parsedElement;
}

std::vector<std::string> ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::vector<std::string>& defaultValue )
{
	std::vector<std::string> parsedElement = defaultValue;

	std::string defaultStringValue = "";
	std::string elementText = ParseXmlAttribute(element, attributeName, defaultStringValue);

	if(elementText != "")
	{
		std::vector<std::string> parsedStrings = SplitStringOnCharacter(elementText, ',');
		for(size_t stringIndex = 0; stringIndex < parsedStrings.size(); stringIndex++)
		{
			parsedElement.push_back(parsedStrings[stringIndex].c_str());
		}
	}	

	return parsedElement;
}

