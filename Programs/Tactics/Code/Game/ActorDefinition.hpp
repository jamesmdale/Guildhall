#pragma once
#include <string>
#include "Engine\Math\AABB2.hpp"
#include <map>
#include <vector>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Math/IntRange.hpp"

struct AbilityStruct
{
	std::string m_id;
	IntRange m_range = IntRange(0, 1); //x = min. y = max;

	//damage for attacks.  Could also be heal amounts for heals
	//x = min. y=max
	IntRange m_valueRange;
	int m_cost;
	int m_delay = 0;;
	IntRange m_areaOfEffectHorizontal = IntRange(0, 0);
	IntRange m_areaOfEffectVertical = IntRange(-999, 999); //ex. assume we have no limit for height.  (Meteor shower ability example)
	float m_blockChanceBuff = 0.0f;
	std::string m_type = "physical";
};

class ActorDefinition
{
public:
	explicit ActorDefinition( const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	AbilityStruct GetAbilityStructById(std::string id);

public:
	std::string m_id;

	//stats
	int m_maxHealth;
	int m_maxJumpHeight;
	int m_maxActionSpeed;
	int m_maxTilesMovement;
	int m_height;

	std::string m_animSetId;
	std::vector<AbilityStruct> m_abilityStructs;

	//previously used a static list in sd1 (adventure)
	static std::map<std::string, ActorDefinition*> s_actorDefinitions;	
};