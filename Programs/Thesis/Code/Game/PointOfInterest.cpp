#include "Game\PointOfInterest.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Map\Map.hpp"
#include "Game\Agent.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"

PointOfInterest::PointOfInterest(ePointOfInterestType poiType, const IntVector2& startingCoordinate, const IntVector2& accessCoordinate, Map* mapReference)
{
	m_type = poiType;
	m_startingCoordinate = startingCoordinate;
	m_accessCoordinate = accessCoordinate;

	m_map = mapReference;

	m_id = m_map->m_pointsOfInterest.size();

	//start stopwatch
	m_refillTimer = new Stopwatch();
	m_refillTimer->SetTimer(g_baseRefillTimePerSecond);
}

PointOfInterest::~PointOfInterest()
{
	m_map = nullptr;
}

void PointOfInterest::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void PointOfInterest::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Rgba tint = Rgba::WHITE;
	switch (m_type)
	{
	case ARMORY_POI_TYPE:
		tint = ARMORY_TINT;
		break;
	case LUMBERYARD_POI_TYPE:
		tint = LUMBER_TINT;
		break;
	case MED_STATION_POI_TYPE:
		tint = MED_TINT;
		break;
	}

	AABB2 bounds;
	bounds.mins = m_map->GetWorldPositionOfMapCoordinate(m_startingCoordinate);
	bounds.maxs = m_map->GetWorldPositionOfMapCoordinate(m_startingCoordinate + IntVector2(2,2));

	theRenderer->SetShader(theRenderer->CreateOrGetShader("agents"));
	theRenderer->DrawAABB(bounds, tint);
	theRenderer->SetShader(theRenderer->CreateOrGetShader("default"));

	theRenderer = nullptr;	
}

IntVector2 PointOfInterest::GetCoordinateBoundsClosestToCoordinate(const IntVector2& coordinate)
{
	//set start to left wall
	IntVector2 closestCoordinate = IntVector2(m_startingCoordinate.x, m_startingCoordinate.y + 1);
	int closestDistance = GetDistanceSquared(m_startingCoordinate, coordinate);
	
	//check lower bottom wall
	IntVector2 boundsCoordinate = IntVector2(m_startingCoordinate.x + 1, m_startingCoordinate.y);
	int boundsDistance = GetDistanceSquared(boundsCoordinate, coordinate);
	if (GetDistanceSquared(boundsCoordinate, coordinate) < closestDistance)
	{
		closestCoordinate = boundsCoordinate;
		closestDistance = boundsDistance;
	}

	//check right wall
	boundsCoordinate = IntVector2(m_startingCoordinate.x + 2, m_startingCoordinate.y + 1);
	boundsDistance = GetDistanceSquared(boundsCoordinate, coordinate);
	if (GetDistanceSquared(boundsCoordinate, coordinate) < closestDistance)
	{
		closestCoordinate = boundsCoordinate;
		closestDistance = boundsDistance;
	}

	//check upper wall
	boundsCoordinate = IntVector2(m_startingCoordinate.x + 1, m_startingCoordinate.y + 2);
	boundsDistance = GetDistanceSquared(boundsCoordinate, coordinate);
	if (GetDistanceSquared(boundsCoordinate, coordinate) < closestDistance)
	{
		closestCoordinate = boundsCoordinate;
		closestDistance = boundsDistance;
	}

	return closestCoordinate;
}
