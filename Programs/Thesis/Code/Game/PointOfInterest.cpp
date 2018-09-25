#include "Game\PointOfInterest.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Map\Map.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"



PointOfInterest::PointOfInterest(ePointOfInterestType poiType, const IntVector2& startingCoordinate, const IntVector2& accessCoordinate)
{
	m_type = poiType;
	m_startingCoordinate = startingCoordinate;
	m_accessCoordinate = accessCoordinate;
}

PointOfInterest::~PointOfInterest()
{
}

void PointOfInterest::Update()
{
	TODO("Add some update functionality later...rotation maybe.")
}

void PointOfInterest::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Rgba tint = Rgba::WHITE;
	switch (m_type)
	{
	case ARMORY_POI_TYPE:
		tint = Rgba(1.f, 0.f, 0.f, 0.5f);
		break;
	case LUMBERYARD_POI_TYPE:
		tint = Rgba(0.f, 1.f, 0.f, 0.5f);
		break;
	case MED_STATION_POI_TYPE:
		tint = Rgba(0.f, 0.f, 1.f, 0.5f);
		break;
	}

	AABB2 bounds;
	bounds.mins = m_mapReference->GetWorldPositionOfMapCoordinate(m_startingCoordinate);
	bounds.maxs = m_mapReference->GetWorldPositionOfMapCoordinate(m_startingCoordinate + IntVector2::ONE);

	theRenderer->DrawAABB(bounds, tint);

	theRenderer = nullptr;	
}
