#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Time\Stopwatch.hpp"

//forward declarations
class Map;
class Agent;

enum ePointOfInterestType
{
	ARMORY_POI_TYPE,
	LUMBERYARD_POI_TYPE,
	MED_STATION_POI_TYPE,
	NUM_POI_TYPES
};

class PointOfInterest
{
public:
	PointOfInterest(ePointOfInterestType poiType, const IntVector2& startingCoordinate, const IntVector2& accessCoordinate, Map* mapReference);
	~PointOfInterest();

	void Update(float deltaSeconds);
	void Render();

	//later add types and stuff

public:
	int m_id = -1;
	Agent* m_agentCurrentlyServing = nullptr;

	IntVector2 m_startingCoordinate;
	IntVector2 m_accessCoordinate;
	ePointOfInterestType m_type;

	Stopwatch* m_refillTimer = new Stopwatch();
	Map* m_mapReference = nullptr;

	//for now all poi are 2x2 blocks with an access point randomly touching one
};
