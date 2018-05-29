#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Stats.hpp"
#include <vector>

class ActorDefinition : EntityDefinition
{
public:
	explicit ActorDefinition( const tinyxml2::XMLElement& element, Renderer& renderer);
	static void Initialize(const std::string& filePath, Renderer& renderer);
public: 
	//list of tile definition member variables
	int m_maxHealth = 100;
	int m_startHealth = 100;
	std::string m_faction = "default";
	std::string m_attackType = "default";
	std::vector<std::string> m_attackableFactions;

	Stats m_minStats;
	Stats m_maxStats;

	bool m_doesHaveActorToActorPhysics = true;
	float m_mass = 1.0;
	bool m_isUnpushable = false;
	static std::map< std::string, ActorDefinition* >	s_actorDefinitions;
};