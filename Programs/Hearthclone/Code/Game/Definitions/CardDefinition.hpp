#pragma once
#include <string>
#include <map>
#include <vector>
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Game\GameCommon.hpp"
#include <string>
#include <vector>

class CardDefinition
{
public:
	explicit CardDefinition(const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	static CardDefinition* GetDefinitionByName(const std::string& cardName);

public :
	eCardType ConvertTypeToCardTypeEnum(std::string cardType);
	eClass ConvertClassToCardClassEnum(std::string cardClass);

public:
	std::string m_name = "";
	eCardType m_type = SPELL_TYPE;
	std::string m_subType = "";
	eClass m_class = NEUTRAL_CLASS;
	std::string m_text = "";

	std::string m_imagePath = "";
	
	int m_cost = 0;
	int m_attack = 0;
	int m_health = 0;
	
	std::vector<std::string> m_tags;

	static std::map<std::string, CardDefinition*> s_cardDefinitions;
};