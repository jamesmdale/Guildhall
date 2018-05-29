#pragma once
#include "Engine/Core/Blackboard.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <string>
#include "Engine/Core/ErrorWarningAssert.hpp"


void Blackboard::PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element )
{
	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute; attribute = attribute->Next())
	{
		m_keyValuePairs.insert(std::pair<std::string, std::string>(std::string(attribute->Name()), std::string(attribute->Value())));
		//TODO:need validation to prevent duplicates
	}	

	//debugger notification
	DebuggerPrintf("Loaded game config!!!");
}
void Blackboard::SetValue( const std::string& keyName, const std::string& newValue )
{
	std::map<std::string, std::string>::iterator value = m_keyValuePairs.find(keyName);
	
	if (value != m_keyValuePairs.end())
	{
		m_keyValuePairs.at(keyName) = newValue;
	}
	else
	{
		std::pair<std::string, std::string>(keyName, newValue);
		m_keyValuePairs.insert(std::pair<std::string, std::string>(keyName, newValue));
	}
}


bool Blackboard::GetValue( const std::string& keyName, bool defaultValue ) const
{
	bool value = defaultValue;
	if(keyName != "")
	{
		if(keyName == "1" || keyName == "true")
		{
			value = true;
		}		

		if(keyName == "0" || keyName == "false")
		{
			value = false;
		}
	}

	return value;
}
int Blackboard::GetValue( const std::string& keyName, int defaultValue ) const
{
	bool value = defaultValue;
	
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value = atoi(valueAsString.c_str());
	}	
	
	return value;
}
float Blackboard::GetValue( const std::string& keyName, float defaultValue ) const
{
	bool value = defaultValue;
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value = (float)atof(keyName.c_str());
	}

	return value;
}
std::string	Blackboard::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::string value = defaultValue;

	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value = keyName;
	}

	return value;
}
std::string	Blackboard::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string value = defaultValue;

	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value = keyName;
	}

	return value;
}
Rgba Blackboard::GetValue( const std::string& keyName, const Rgba& defaultValue ) const
{
	Rgba value = defaultValue;

	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value.SetFromText(keyName.c_str());
	}

	return value;
}
Vector2	Blackboard::GetValue( const std::string& keyName, const Vector2& defaultValue ) const
{
	Vector2 value = defaultValue;
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value.SetFromText(keyName.c_str());
	}

	return value;
}
IntVector2	Blackboard::GetValue( const std::string& keyName, const IntVector2& defaultValue ) const
{
	IntVector2 value = defaultValue;
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value.SetFromText(keyName.c_str());
	}

	return value;
}
FloatRange Blackboard::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	FloatRange value = defaultValue;
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value.SetFromText(keyName.c_str());
	}

	return value;
}
IntRange Blackboard::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	IntRange value = defaultValue;
	auto mapVal = m_keyValuePairs.find(keyName);

	if (mapVal != m_keyValuePairs.end())
	{
		std::string valueAsString = m_keyValuePairs.at(keyName);
		value.SetFromText(keyName.c_str());
	}

	return value;
}