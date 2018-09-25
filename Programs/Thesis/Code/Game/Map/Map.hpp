#pragma once
#include <string>
#include <vector>
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Utility\Grid.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\Tile.hpp"
#include "Game\PointOfInterest.hpp"

class Map
{
public:
	explicit Map(MapDefinition* definition, const std::string& mapName, RenderScene2D* renderScene);

	Map::~Map();

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

	//Conversion functions for Tile Coordinates to World Coordinates
	IntVector2 GetTileCoordinateOfPosition(const Vector2& position);
	Vector2 GetWorldPositionOfMapCoordinate(const IntVector2& position);

	//helpers
	bool CheckIsPositionValid(const Vector2& position);
	bool CheckIsCoordianteValid(const IntVector2& coordinate);
	Vector2 GetRandomNonBlockedPositionInMapBounds();
	IntVector2 GetRandomNonBlockedCoordinateInMapBounds();
	Grid<int>* GetAsGrid();
	bool IsTileBlockingAtCoordinate(const IntVector2& coordinate);
	Tile * GetTileAtCoordinate(const IntVector2 & coordinate);

	PointOfInterest* GeneratePointOfInterest(ePointOfInterestType poiType);


public:
	std::string m_name;
	IntVector2 m_dimensions;
	MapDefinition* m_mapDefinition = nullptr;
	std::vector<Tile*> m_tiles;
	AABB2 m_mapWorldBounds;

	//RenderScene2D* m_renderScene = nullptr;
private:
	bool m_isFullMapView = false;
};