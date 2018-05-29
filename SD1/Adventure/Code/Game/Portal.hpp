#pragma once
#include "Game/Entity.hpp"
#include "Game/PortalDefinition.hpp"
#include "Engine/Math/Vector2.hpp"

class Map;
class Portal : public Entity
{
public:
	Portal(PortalDefinition* portalDef, std::string onTileType, std::string destinationMapName, std::string destinationTileType, PortalDefinition* reciprocalType);
	~Portal(){};
	//void Render();
public:
	PortalDefinition* m_portalDef = nullptr;

	std::string m_onTileType = "default";
	std::string m_destinationMapName = "default";
	std::string m_destinationTileType = "default";
	PortalDefinition* m_reciprocalType = nullptr;

	Map* m_myMap = nullptr;
	Map* m_destinationMap = nullptr;
	Portal* m_destinationPortal = nullptr;
	Vector2 m_destinationCoordinates = Vector2(0,0);

	bool m_isPlayerCollisionDisabled = false;
	
};
