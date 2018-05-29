#include "Game/MapGenStep_SpawnItem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapGenStep_SpawnItem::MapGenStep_SpawnItem( const tinyxml2::XMLElement& generationStepElement )
	: MapGenStep( generationStepElement )
{
	m_name = generationStepElement.Name();
	std::string itemTypeNameParsedString = ParseXmlAttribute(generationStepElement, "type", std::string("default"));
	m_itemType = ItemDefinition::s_itemDefinitions[itemTypeNameParsedString];

	m_iterations = ParseXmlAttribute(generationStepElement, "iterations", m_iterations);

	m_chanceToRun = ParseXmlAttribute(generationStepElement, "chanceToRun", m_chanceToRun);

	std::string tileTypeNameParsedString = ParseXmlAttribute(generationStepElement, "onTile", std::string("default"));
	m_onTileType = TileDefinition::s_definitions[tileTypeNameParsedString];
}

void MapGenStep_SpawnItem::Run( Map& map )
{
	std::vector<Tile*> matchingTypeTiles;
	if(m_onTileType != nullptr)
	{		
		for(size_t tileIndex = 0; tileIndex < map.m_tiles.size(); tileIndex++)
		{	
			if(map.m_tiles[tileIndex].m_tileDef == m_onTileType)
			{
				matchingTypeTiles.push_back(&map.m_tiles[tileIndex]);
			}
		}
	}

	int iterations = GetRandomIntInRange(m_iterations.min, m_iterations.max);
	for(int iterationIndex = 0; iterationIndex < iterations; iterationIndex++)
	{
		float chanceToRun = GetRandomFloatZeroToOne();
		if(chanceToRun <= m_chanceToRun) //if 1.0, always will run.
		{	
			if(m_onTileType != nullptr)
			{
				bool isItemSpawned = false;
				int breakCounter = 0; //just a fail safe if we can't find a tile. Shouldn't happen.
				while(!isItemSpawned)
				{				
					GUARANTEE_OR_DIE(breakCounter < 10000, "Valid tile for" + ((EntityDefinition*)m_itemType)->m_name + " to spawn on not found");

					breakCounter++;
					int randomTileIndex = GetRandomIntInRange(0, (int)matchingTypeTiles.size()-1);
					if(CheckIfValidSpawnLocation(*map.m_tiles[randomTileIndex].m_tileDef))
					{				
						//TODO: could tag tiles in the future to determine if something has already been spawned on it (an actor or an item, etc PROBABLY ENTITY ONLY)	
						map.SpawnItemByDefinition(m_itemType, Vector2((float)matchingTypeTiles[randomTileIndex]->m_tileCoords.x + 0.5f, (float)matchingTypeTiles[randomTileIndex]->m_tileCoords.y + 0.5f));
						isItemSpawned = true;
					}
				}
			}
			else
			{
				bool isItemSpawned = false;
				int breakCounter = 0; //just a fail safe if we can't find a tile. Shouldn't happen.
				while(!isItemSpawned)
				{				
					GUARANTEE_OR_DIE(breakCounter < 10000, "Valid tile for" + ((EntityDefinition*)m_itemType)->m_name + " to spawn on not found");

					breakCounter++;
					int randomTileIndex = GetRandomIntInRange(0, (int)map.m_tiles.size());
					if(CheckIfValidSpawnLocation(*map.m_tiles[randomTileIndex].m_tileDef))
					{
						//TODO: could tag tiles in the future to determine if something has already been spawned on it (an actor or an item, etc PROBABLY ENTITY ONLY)
						map.SpawnItemByDefinition(m_itemType, Vector2((float)matchingTypeTiles[randomTileIndex]->m_tileCoords.x + 0.5f, (float)matchingTypeTiles[randomTileIndex]->m_tileCoords.y + 0.5f));
						isItemSpawned = true;
					}
				}				
			}
		}
	}	
}

bool MapGenStep_SpawnItem::CheckIfValidSpawnLocation(const TileDefinition& tileDef)
{
	bool isValid = true;

	//TODO: ADD more checks here as necessary. Also might be good to put into a method on map or tile to be reused.
	if(!tileDef.m_allowsWalking)
	{
		isValid = false;
	}

	return isValid;
}
