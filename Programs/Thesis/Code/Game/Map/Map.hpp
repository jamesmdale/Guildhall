#pragma once
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\Tile.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Utility\Grid.hpp"
#include <string>
#include <vector>


//forward declarations
enum ePointOfInterestType;
class Agent;
class PointOfInterest;
class Bombardment;
class Stopwatch;
class Mesh;

class Map
{
public:
	explicit Map(MapDefinition* definition, const std::string& mapName, RenderScene2D* renderScene);

	Map::~Map();

	void Initialize();

	void Update(float deltaSeconds);
	void Render();

	void SetMapType(MapDefinition* newMapDefintion) { m_mapDefinition = newMapDefintion; }
	IntVector2 GetDimensions() { return m_dimensions; }

	//optimized mesh generation
	void CreateMapMesh();
	Mesh* CreateDynamicAgentMesh();
	Mesh* CreateDynamicBombardmentMesh();	

	//Cleanup functions
	void DeleteDeadEntities();

	//General functions
	void UpdatePlayerInput();
	int GetActorIndex();

	//Conversion functions for Tile Coordinates to World Coordinates
	IntVector2 GetTileCoordinateOfPosition(const Vector2& position);
	Vector2 GetWorldPositionOfMapCoordinate(const IntVector2& coordinate);

	//helpers
	bool CheckIsPositionValid(const Vector2& position);
	bool CheckIsCoordianteValid(const IntVector2& coordinate);
	Vector2 GetRandomNonBlockedPositionInMapBounds();
	IntVector2 GetRandomNonBlockedCoordinateInMapBounds();
	IntVector2 GetRandomCoordinateInMapBounds();
	Grid<int>* GetAsGrid();
	bool IsTileBlockingAtCoordinate(const IntVector2& coordinate);
	Tile* GetTileAtCoordinate(const IntVector2& coordinate);
	Agent* GetAgentById(int agentId);

	float GetAveragePOIHealth();
	float GetAverageAgentHealth();

	//point of interest helpers
	PointOfInterest* GeneratePointOfInterest(int poiType);
	PointOfInterest* GetPointOfInterestById(int poiId);	

	//bombardment collision
	void DetectBombardmentToAgentCollision(Bombardment* bombardment);
	void DetectBombardmentToPOICollision(Bombardment* bombardment);


public:
	std::string m_name;
	IntVector2 m_dimensions;
	MapDefinition* m_mapDefinition = nullptr;
	std::vector<Tile*> m_tiles;
	AABB2 m_mapWorldBounds;

	std::vector<Agent*> m_agents;
	std::vector<PointOfInterest*> m_pointsOfInterest;
	std::vector<Bombardment*> m_activeBombardments;

	//meshes for rendering
	Mesh* m_mapMesh = nullptr;

	Stopwatch* m_bombardmentTimer = nullptr;
	Stopwatch* m_threatTimer = nullptr;

	float m_threat = 0.f;

private:
	bool m_isFullMapView = false;
};