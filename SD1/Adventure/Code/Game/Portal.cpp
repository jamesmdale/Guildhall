#include "Game/Portal.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
Portal::Portal(PortalDefinition* portalDef, std::string onTileType, std::string destinationMapName, std::string destinationTileType, PortalDefinition* reciprocalType) : Entity((EntityDefinition*)portalDef)
{
	m_portalDef = portalDef;
	m_onTileType = onTileType;

	m_destinationMapName = destinationMapName;
	m_destinationTileType = destinationTileType;
	m_reciprocalType = reciprocalType;
}