#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "engine/renderer/SpriteAnimSetDefinition.hpp"

enum EntityType
{
	ACTOR,
	PROJECTILE,
	PORTAL,
	NUM_ENTITY_TYPES
};

class EntityDefinition
{
public:
	/*EntityDefinition();
	~EntityDefinition( );*/
	explicit EntityDefinition( const tinyxml2::XMLElement& element,  Renderer& renderer );
	static void Initialize(const std::string& filePath, Renderer& renderer);
public: 
	//list of tile definition member variables
	std::string m_name = "default";
	std::string m_startingTags;
	int m_entityType;
	float m_physicsRadius = 0.0f;
	float m_drawRadius = 0.0f;
	AABB2 m_localDrawBounds = AABB2(Vector2(0,0), Vector2(0,0));	

	//TODO: add more movement speeds/types here (swimming etc)
	bool m_doesWalk;
	bool m_doesFly;
	float m_walkSpeed = 0.0f;
	float m_flySpeed = 0.0f;	
	
	SpriteAnimSetDefinition* m_animSetDef = nullptr;
	static std::map< std::string, EntityDefinition* >s_entityDefinitions;
};