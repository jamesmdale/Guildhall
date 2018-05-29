#pragma once
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Game/MapGenStep.hpp"
#include "Engine/Math/IntRange.hpp"

class MapGenStep_SpawnActor : public MapGenStep
{
public:
	explicit MapGenStep_SpawnActor( const tinyxml2::XMLElement& genStepXmlElement );
	~MapGenStep_SpawnActor() {};
	void Run( Map& map ); // "pure virtual", MUST be overridden by subclasses

	bool CheckIfValidSpawnLocation(const TileDefinition& tileDef);

private:
	std::string		m_name;
	ActorDefinition* m_actorType = nullptr;
	TileDefinition* m_onTileType = nullptr;
	float m_chanceToRun = 1.f;
	IntRange m_iterations = IntRange(1);

};
