#pragma once
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Game/MapGenStep.hpp"
#include "Engine/Math/IntRange.hpp"

class MapGenStep_SpawnItem : public MapGenStep
{
public:
	explicit MapGenStep_SpawnItem( const tinyxml2::XMLElement& genStepXmlElement );
	~MapGenStep_SpawnItem() {};
	void Run( Map& map ); // "pure virtual", MUST be overridden by subclasses

	bool CheckIfValidSpawnLocation(const TileDefinition& tileDef);

private:
	std::string		m_name;
	ItemDefinition* m_itemType = nullptr;
	TileDefinition* m_onTileType = nullptr;
	float m_chanceToRun = 1.f;
	IntRange m_iterations = IntRange(1);
};
