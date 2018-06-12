#pragma once
#include <string>
#include <map>
#include <vector>
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Game\GameCommon.hpp"
#include <string>
#include <vector>

class DeckDefinition
{
public:
	explicit DeckDefinition(const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	static DeckDefinition* GetDefinitionByName(const std::string& deckName);

public :
	std::string m_deckName;
	std::vector<std::string> m_cardNames;

public:
	static std::map<std::string, DeckDefinition*> s_deckDefinitions;
};