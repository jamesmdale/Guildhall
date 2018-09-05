#pragma once
#include <string>
#include "Engine\Math\IntVector2.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include <vector>
#include "Game\Map\Tile.hpp"

class Map
{
public:
	explicit Map(MapDefinition* definition, const std::string& mapName);
	void SetMapType(MapDefinition* newMapDefintion) { m_mapDefinition = newMapDefintion; }
	IntVector2 GetDimensions() { return m_dimensions; }

	void Render();
	void Update(float timeDelta);

	//spawn functions
	//void SpawnActorByName(std::string actorType, Vector2 spawnPosition);
	//void SpawnActorByDefinition(ActorDefinition* actorType, Vector2 spawnPosition);

	//Collision detections
	
	/*void ActorToTileCollision();
	void DetectActorToTileCollision(int entityIndex, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection);
	void ActorToPortalCollision();
	void ActorToActorCollision();
	void ActorToItemCollision();
	void ActorToActorAttackCollision();*/

	//Cleanup functions
	void DeleteDeadEntities();

	//General functions
	void UpdatePlayerInput();
	int GetActorIndex();



	//void GenerateMap(const std::string& definitionName, const std::string& mapName);

public:
	std::string m_Name;
	IntVector2 m_dimensions;
	MapDefinition* m_mapDefinition = nullptr;
	std::vector<Tile> m_tiles;
private:
	bool m_isFullMapView = false;
};