#include "Game/MapGenStep_CellularAutomata.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapGenStep_CellularAutomata::MapGenStep_CellularAutomata( const tinyxml2::XMLElement& generationStepElement )
	: MapGenStep( generationStepElement )
{
	m_name = generationStepElement.Name();
	std::string ifTileType = ParseXmlAttribute(generationStepElement, "ifType", std::string("default"));
	m_ifType = TileDefinition::s_definitions[ifTileType];

	std::string changeToType = ParseXmlAttribute(generationStepElement, "changeToType", std::string("default"));
	m_changeToType = TileDefinition::s_definitions[changeToType];

	std::string ifNeighborType = ParseXmlAttribute(generationStepElement, "ifNeighborType", std::string("default"));
	m_ifNeighborType = TileDefinition::s_definitions[ifNeighborType];

	m_chanceToMutate = ParseXmlAttribute(generationStepElement, "chanceToMutate", m_chanceToMutate);

	m_ifNeighborCount = ParseXmlAttribute(generationStepElement, "ifNeighborCount", m_ifNeighborCount);
	if(m_ifNeighborCount.max > 8)
	{
		m_ifNeighborCount.max = 8;
	}
	if(m_ifNeighborCount.min < 0)
	{
		m_ifNeighborCount.min = 0;
	}
}

void MapGenStep_CellularAutomata::Run( Map& map )
{
	std::vector<int> tileIndexesToChange;
	
	for(int tileIndex = 0; tileIndex < (int)map.m_tiles.size(); tileIndex++)
	{	
		if(map.m_tiles[tileIndex].m_tileDef == m_ifType)
		{
			float randomChance = GetRandomFloatZeroToOne();
			if(randomChance <= m_chanceToMutate)
			{
				if(m_ifNeighborType != nullptr)
				{				
					int neighborCount = 0;
					std::vector<int> neighboringTiles = GetNeighboringTiles(tileIndex, map);
					for(size_t neighboringTileIndex = 0; neighboringTileIndex < neighboringTiles.size(); neighboringTileIndex++)
					{
						int neighborIndex = neighboringTiles[neighboringTileIndex];
						if(map.m_tiles[neighborIndex].m_tileDef == m_ifNeighborType)
						{
							neighborCount++;
						}
					}
					if(m_ifNeighborCount.min == -1 && m_ifNeighborCount.max == -1)
					{
						if(neighborCount > 0)
						{
							tileIndexesToChange.push_back(tileIndex);
						}
					}
					else if(neighborCount >= m_ifNeighborCount.min && neighborCount <= m_ifNeighborCount.max)
					{
						tileIndexesToChange.push_back(tileIndex);
					}
				}
				else
				{
					tileIndexesToChange.push_back(tileIndex);
				}				
			}		
		}			
	}

	for(size_t tileIndex = 0; tileIndex < tileIndexesToChange.size(); tileIndex++)
	{
		int currentIndex = tileIndexesToChange[tileIndex];

		map.m_tiles[currentIndex].m_tileDef = m_changeToType;
	}
}

std::vector<int> MapGenStep_CellularAutomata::GetNeighboringTiles(int currentTileIndex, const Map& map)
{
	std::vector<int> neighboringTilesIndexes;	
	IntVector2 currentTileCoords = map.m_tiles[currentTileIndex].m_tileCoords;


	IntVector2 eastTile = IntVector2(currentTileCoords.x + 1, currentTileCoords.y);
	int eastIndex = (eastTile.x * map.m_dimensions.y) + eastTile.y;

	if(eastIndex > 0 && eastIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(eastIndex);
	}
	

	IntVector2 westTile = IntVector2(currentTileCoords.x - 1 , currentTileCoords.y);
	int westIndex = (westTile.x * map.m_dimensions.y) + westTile.y;
	if(westIndex > 0 && westIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(westIndex);
	}

	IntVector2 northTile = IntVector2(currentTileCoords.x, currentTileCoords.y + 1);
	int northIndex = (northTile.x * map.m_dimensions.y) + northTile.y;
	if(northIndex > 0 && northIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(northIndex);
	}

	IntVector2 southTile = IntVector2(currentTileCoords.x, currentTileCoords.y - 1);
	int southIndex = (southTile.x * map.m_dimensions.y) + southTile.y;
	if(southIndex > 0 && southIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(southIndex);
	}

	IntVector2 northEastTile = IntVector2(currentTileCoords.x + 1, currentTileCoords.y + 1);
	int northEastIndex = (northEastTile.x * map.m_dimensions.y) + northEastTile.y;
	if(northEastIndex > 0 && northEastIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(northEastIndex);
	}

	IntVector2 northWestTile = IntVector2(currentTileCoords.x - 1, currentTileCoords.y + 1);
	int northWestIndex = (northWestTile.x * map.m_dimensions.y) + northWestTile.y;
	if(northWestIndex > 0 && northWestIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(northWestIndex);
	}

	IntVector2 southEastTile = IntVector2(currentTileCoords.x + 1, currentTileCoords.y - 1);
	int southEastIndex = (southEastTile.x * map.m_dimensions.y) + southEastTile.y;
	if(southEastIndex > 0 && southEastIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(southEastIndex);
	}

	IntVector2 southWestTile = IntVector2(currentTileCoords.x - 1, currentTileCoords.y - 1);
	int southWestIndex = (southWestTile.x * map.m_dimensions.y) + southWestTile.y;
	if(southWestIndex > 0 && southWestIndex < (int)map.m_tiles.size())
	{
		neighboringTilesIndexes.push_back(southWestIndex);
	}

	return neighboringTilesIndexes;
}