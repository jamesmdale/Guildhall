#pragma once
#include <string>
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\Vector2.hpp"
#include <map>
#include <vector>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine\Core\EngineCommon.hpp"


class EncounterDefinition
{
public:
	explicit EncounterDefinition( const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);

public:
	std::string m_id;
	std::string m_mapImageName;
	int m_mapMaxheight = 1;

	std::vector<std::string> m_redFactionActors;
	std::vector<std::string> m_blueFactionActors;
	int m_numBlueFaction = 1;

	TODO("Add more options here for types of actors, etc");

	//previously used a static list in sd1 (adventure)
	static std::map<std::string, EncounterDefinition*> s_encounterDefinitions;	
};