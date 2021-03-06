#include "Game/MapGenStep_Mutate.hpp"
#include "Game/Tags.hpp"

MapGenStep_Mutate::MapGenStep_Mutate( const tinyxml2::XMLElement& generationStepElement )
	: MapGenStep( generationStepElement )
{
	m_name = generationStepElement.Name();
	m_chanceToMutate = ParseXmlAttribute(generationStepElement, "chanceToMutate", m_chanceToMutate);

	std::string defaultMutateTile = "Water";
	std::string mutateTileNameParsedString = ParseXmlAttribute(generationStepElement, "mutateTile", defaultMutateTile);
	m_mutateTileDef = TileDefinition::s_definitions[mutateTileNameParsedString];
}

void MapGenStep_Mutate::Run( Map& map )
{
	for(int tileIndex = 0; tileIndex < (int)map.m_tiles.size(); tileIndex++)
	{		
		float randomChance = GetRandomFloatZeroToOne();
		if(randomChance < m_chanceToMutate)
		{
			if(m_ifTags != "")
			{
				if(map.m_tiles[tileIndex].m_tileInfo->m_tags->HasTags(m_ifTags))
				{
					map.m_tiles[tileIndex].m_tileDef = m_mutateTileDef;
					map.m_tiles[tileIndex].m_tileInfo->m_tags->SetOrRemoveTags(m_setTags);
				}						
			}
			else
			{
				map.m_tiles[tileIndex].m_tileDef = m_mutateTileDef;
				map.m_tiles[tileIndex].m_tileInfo->m_tags->SetOrRemoveTags(m_setTags);
			}
			
		}			
	}
}
