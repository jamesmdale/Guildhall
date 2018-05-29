#include "Game\Map.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\Image.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Game\HeatMap.hpp"
#include "Game\GameCommon.hpp"
#include "Game\ActorDefinition.hpp"


void Map::Update(float deltaSeconds)
{
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
	{
		m_actors[actorIndex]->Update(deltaSeconds);
	}
}

void Map::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	//all blocks will be drawn with the same texture
	theRenderer->SetTexture(*m_blockDefinitionTexture);

	theRenderer->EnableDepth(LESS, true);
	//draw all blocks
	for (int blockIndexX = 0; blockIndexX < GetWidth(); blockIndexX++) //x axis
	{
		for (int blockIndexZ = 0; blockIndexZ < GetDepth(); blockIndexZ++) //z axis
		{
			for(int blockIndexY = 0; blockIndexY < GetHeight(); blockIndexY++) //y axis
			{
				Block block = *GetBlock(IntVector3(blockIndexX, blockIndexY, blockIndexZ));
				
				if(block.m_isEmpty == false)
				{
					Vector3 blockCenterPosition = Vector3(blockIndexX, blockIndexY,  blockIndexZ);

					theRenderer->DrawCube(blockCenterPosition, Vector3::ONE, Rgba::WHITE, block.m_definition->m_uvTop, block.m_definition->m_uvSide, block.m_definition->m_uvBottom);
				}				
			}
		}
	}


	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
	{
		m_actors[actorIndex]->Render();
	}

	theRenderer->EnableDepth(LESS, true);

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	if((int)m_pathingDrawTilesIndices.size() > 0)
	{
		Rgba pathingDrawColor = Rgba::LIGHT_BLUE;
		Rgba pathingDrawColorXray = Rgba::LIGHT_WHITE_TRANSPARENT;
		if(m_encounterReference->m_isInCombatState)
		{
			pathingDrawColor = Rgba::ORANGE;
			pathingDrawColorXray = Rgba::LIGHT_ORANGE_TRANSPARENT;
		}
		theRenderer->EnableDepth(LESS, true);
		for(int tileIndex = 0; tileIndex < (int)m_pathingDrawTilesIndices.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_pathingDrawTilesIndices[tileIndex]), pathingDrawColor);
		}

		theRenderer->EnableDepth(GREATER, true);
		for(int tileIndex = 0; tileIndex < (int)m_pathingDrawTilesIndices.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_pathingDrawTilesIndices[tileIndex]), pathingDrawColorXray);
		}
	}
	//set texture for actors and for cursors

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	if((int)m_areaOfEffectDrawTilesIndices.size() > 0)
	{
		Rgba aoeDrawColor = Rgba::PINK;
		Rgba aoeDrawColorXray = Rgba::LIGHT_PINK_TRANSPARENT;

		theRenderer->EnableDepth(LESS, true);
		for(int tileIndex = 0; tileIndex < (int)m_areaOfEffectDrawTilesIndices.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_areaOfEffectDrawTilesIndices[tileIndex]), aoeDrawColor);
		}

		theRenderer->EnableDepth(GREATER, true);
		for(int tileIndex = 0; tileIndex < (int)m_areaOfEffectDrawTilesIndices.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_areaOfEffectDrawTilesIndices[tileIndex]), aoeDrawColorXray);
		}
	}

	////debug for path tiles for movement
	/*if((int)m_pathTiles.size() > 0)
	{
		Rgba aoeDrawColor = Rgba::PINK;
		Rgba aoeDrawColorXray = Rgba::LIGHT_PINK_TRANSPARENT;

		theRenderer->EnableDepth(LESS, true);
		for(int tileIndex = 0; tileIndex < (int)m_pathTiles.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_pathTiles[tileIndex]), aoeDrawColor);
		}

		theRenderer->EnableDepth(GREATER, true);
		for(int tileIndex = 0; tileIndex < (int)m_pathTiles.size(); tileIndex++)
		{
			DrawTileHighlightAtPosition(GetBlockPositionByIndexXZ(m_pathTiles[tileIndex]), aoeDrawColorXray);
		}
	}*/

	theRenderer->EnableDepth(LESS, true);
	//draw a yellow cursor if the cursor is located somewhere in the cursor position path
	if(m_isCursorPositionInPathingIndices)
	{
		Rgba yellow = Rgba::YELLOW;
		Rgba yellowTransparent = yellow;
		yellowTransparent.a = 150;	

		theRenderer->EnableDepth(LESS, true);
		DrawTileHighlightAtPosition(IntVector2(m_cursorPosition.x, m_cursorPosition.y + 0.1f), yellow);	

		theRenderer->EnableDepth(GREATER, true);
		DrawTileHighlightAtPosition(IntVector2(m_cursorPosition.x, m_cursorPosition.y + 0.1f), yellowTransparent);			
	}


	//draw a red cursor if we are outside of a place we can move or attack depending on the option selected
	else if(m_isCursorPositionInPathingIndices == false && m_cursorPosition != m_currentSelectedActorPosition)
	{
		Rgba red = Rgba::RED;
		Rgba redTransparent = red;
		redTransparent.a = .5f;	

		theRenderer->EnableDepth(GREATER, true);
		DrawTileHighlightAtPosition(IntVector2(m_cursorPosition.x, m_cursorPosition.y), Rgba::LIGHT_WHITE_TRANSPARENT);

		theRenderer->EnableDepth(LESS, true);
		DrawTileHighlightAtPosition(IntVector2(m_cursorPosition.x, m_cursorPosition.y), red);		
	}
	else //we are on the current player character so we should jsut draw a white box.
	{
		theRenderer->EnableDepth(LESS, true);
		DrawTileHighlightAtPosition(m_currentSelectedActorPosition, Rgba::WHITE);

		theRenderer->EnableDepth(GREATER, true);
		DrawTileHighlightAtPosition(m_currentSelectedActorPosition, Rgba::LIGHT_WHITE_TRANSPARENT);
	}

	theRenderer->EnableDepth(GREATER, true);
	DrawTileHighlightAtPosition(m_currentSelectedActorPosition, Rgba::LIGHT_WHITE_TRANSPARENT);

	

	theRenderer = nullptr;
	delete(theRenderer);
}

void Map::RegisterBlockType(std::string name, IntVector2 tileTop, IntVector2 tileSide, IntVector2 tileBottom)
{
	BlockDefinition* newDef = new BlockDefinition();

	newDef->m_spriteSheet = new SpriteSheet(*m_blockDefinitionTexture, 32, 32);
	newDef->m_name = name;

	newDef->m_uvSide = newDef->m_spriteSheet->GetTexCoordsForSpriteCoords(tileSide);
	newDef->m_uvTop = newDef->m_spriteSheet->GetTexCoordsForSpriteCoords(tileTop);
	newDef->m_uvBottom = newDef->m_spriteSheet->GetTexCoordsForSpriteCoords(tileBottom);

	m_blockDefinitions.push_back(newDef);
}

BlockDefinition* Map::GetBlockType(std::string name)
{
	for(int definitionIndex = 0; definitionIndex < (int)m_blockDefinitions.size(); definitionIndex++)
	{
		if(m_blockDefinitions[definitionIndex]->m_name == name)
		{
			return m_blockDefinitions[definitionIndex];
		}
	}

	//if we didn't find it return nullptr
	return nullptr;
}


TODO("For now, maps are hardcoded to be 16x16x6.  Fix this later when having more dynamic maps and sizes.")
void Map::GenerateMap(Encounter* encounter)
{
	m_encounterReference = encounter;
	Renderer* theRenderer = Renderer::GetInstance();

	TODO("Later on this will be set off to be done in the definition class itself");
	m_blockDefinitionTexture = theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	RegisterBlockType("ground", IntVector2(12, 0), IntVector2(18, 1), IntVector2(16, 1));

	m_mapFilePath = encounter->m_definition->m_mapImageName;
	Image* mapImage = new Image(m_mapFilePath);
	
	IntVector2 xzDimensions = mapImage->GetDimensions();
	m_dimensions = IntVector3(xzDimensions.x, encounter->m_definition->m_mapMaxheight, xzDimensions.y);

	int numBlocks = GetBlockCount();
	m_blocks = new Block[numBlocks];

	std::vector< Rgba > texels = mapImage->GetTexels();

	TODO("Have a counter just so you can more easily test whether or not your get index is functional");
	int counter = 0;

	for (int blockIndexX = 0; blockIndexX < GetWidth(); blockIndexX++)
	{
		for (int blockIndexZ = 0; blockIndexZ < GetDepth(); blockIndexZ++)
		{
			int mapPositionHeight = (int)(RangeMapFloat((float)mapImage->GetTexel(blockIndexX, blockIndexZ).r, 0.f, 255.f, 0.f, (float)encounter->m_definition->m_mapMaxheight));
			for(int blockIndexY = 0; blockIndexY < GetHeight(); blockIndexY++)
			{								
				if(blockIndexY < mapPositionHeight)
				{
					int index = GetBlockIndexByPosition(IntVector3(blockIndexX, blockIndexY, blockIndexZ));
					Block& block = m_blocks[index];
					//block.m_index = counter;
					block.m_isEmpty = false;
					block.m_definition = GetBlockType("ground");
				}	
				else
				{
					int index = GetBlockIndexByPosition(IntVector3(blockIndexX, blockIndexY, blockIndexZ));
					Block& block = m_blocks[index];
					block.m_index = counter;
				}
				counter++;
			}
		}
	}	
	DebuggerPrintf("Created %i blocks", counter);
	GenerateActors(*encounter);

	for(int blockIndexX = 0; blockIndexX < m_dimensions.x; blockIndexX++)
	{
		for(int blockIndexZ = 0; blockIndexZ < m_dimensions.z; blockIndexZ++)
		{
			m_heights.push_back(GetHeightForPositionXZ(IntVector2(blockIndexX, blockIndexZ)));
		}		
	}

	theRenderer = nullptr;
	delete(theRenderer);

	return;
}

void Map::GenerateActors(const Encounter& encounter)
{
	int numBlueActors = (int)encounter.m_definition->m_blueFactionActors.size();
	int numRedActors = (int)encounter.m_definition->m_redFactionActors.size();

	IntVector2 halfOfMap = IntVector2(RoundToNearestInt(m_dimensions.x / 2), RoundToNearestInt(m_dimensions.z / 2));

	for(int actorCount = 0; actorCount < numBlueActors; actorCount++)
	{
		std::string actorDefId = encounter.m_definition->m_blueFactionActors[actorCount];

		IntVector2 randomSpawnLocationBlueFaction;

		bool validSpawn = false;
		while(!validSpawn)
		{
			randomSpawnLocationBlueFaction = IntVector2(GetRandomIntInRange(0, halfOfMap.x), GetRandomIntInRange(0, halfOfMap.y));
			bool isLocationTaken = false;
			for(int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
			{
				if(randomSpawnLocationBlueFaction == m_actors[actorIndex]->GetPositionInXZ())
				{
					isLocationTaken = true;
					break;
				}
			}

			if(isLocationTaken == false)
			{
				validSpawn = true;
			}	

			if(GetBlockIndexByPositionXZ(randomSpawnLocationBlueFaction) > (m_dimensions.x * m_dimensions.z) || GetBlockIndexByPositionXZ(randomSpawnLocationBlueFaction) < 0)
			{
				validSpawn = false;				
			}
		}

		std::map<std::string, ActorDefinition*>::iterator actorDefIterator = ActorDefinition::s_actorDefinitions.find(actorDefId);
		if (actorDefIterator != ActorDefinition::s_actorDefinitions.end())
		{
			m_actors.push_back(new Actor(randomSpawnLocationBlueFaction, BLUE_ACTOR_FACTION, actorDefIterator->second, this, MAP_NORTH));
		}			
	}

	for(int actorCount = 0; actorCount < numRedActors; actorCount++)
	{
		std::string actorDefId = encounter.m_definition->m_redFactionActors[actorCount];
		IntVector2 randomSpawnLocationRedFaction;

		bool validSpawn = false;
		while(!validSpawn)
		{
			randomSpawnLocationRedFaction = IntVector2(GetRandomIntInRange(0, halfOfMap.x), GetRandomIntInRange(0, halfOfMap.y));
			bool isLocationTaken = false;
			for(int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
			{
				if(randomSpawnLocationRedFaction == m_actors[actorIndex]->GetPositionInXZ())
				{
					isLocationTaken = true;
					break;
				}
			}
			if(isLocationTaken == false)
			{
				validSpawn = true;
			}

			if(GetBlockIndexByPositionXZ(randomSpawnLocationRedFaction) > (m_dimensions.x * m_dimensions.z) || GetBlockIndexByPositionXZ(randomSpawnLocationRedFaction) < 0)
			{
				validSpawn = false;				
			}
		}

		std::map<std::string, ActorDefinition*>::iterator actorDefIterator = ActorDefinition::s_actorDefinitions.find(actorDefId);
		if (actorDefIterator != ActorDefinition::s_actorDefinitions.end())
		{
			m_actors.push_back(new Actor(randomSpawnLocationRedFaction, RED_ACTOR_FACTION, actorDefIterator->second, this, MAP_NORTH));
		}			
	}

	//debug sprite animations
	//for(int i = 0; i < (int)m_actors.size(); i++)
	//{
	//	m_actors[i]->m_animationSet->SetCurrentAnim("walk");
	//}
}

int Map::GetBlockIndexByPosition(const IntVector3& position)
{
	if ((position.x < 0) || (position.x  >= m_dimensions.x) || (position.z < 0) || (position.z >= m_dimensions.z)) 
	{
		return -1;
	}

	return position.y + position.z * GetHeight() + position.x * GetHeight() * GetDepth();
}

int Map::GetBlockIndexByPositionXZ(const IntVector2& positionXZ)
{
	if ((positionXZ.x < 0) || (positionXZ.x  >= m_dimensions.x) || (positionXZ.y < 0) || (positionXZ.y >= m_dimensions.z)) 
	{
		return -1;
	}

	return positionXZ.y * m_dimensions.z + positionXZ.x;
}

IntVector2 Map::GetBlockPositionByIndexXZ(const int& index)
{
	int z = index / m_dimensions.x; 
	int x = index % m_dimensions.x; 
	
	return IntVector2(x, z);
}

Block* Map::GetBlock(const IntVector3& position)
{
	return &m_blocks[GetBlockIndexByPosition(position)];
}

TODO("Finish set blcok");
Block* Map::SetBlock(const IntVector3& position)
{
	return nullptr;
}

int Map::GetHeightForPositionXZ(IntVector2 baseBlockPosition)
{
	for(int heightIndex = 0; heightIndex < GetHeight(); heightIndex++)
	{
		if(GetBlock(IntVector3(baseBlockPosition.x, heightIndex, baseBlockPosition.y))->m_isEmpty)
		{
			return heightIndex;
		}
	}

	return GetHeight();
}

void Map::DrawTileHighlightAtPosition(IntVector2 blockPosition, Rgba color)
{
	Renderer* theRenderer = Renderer::GetInstance();

	int height = GetHeightForPositionXZ(blockPosition);

	Vector3 cursorPosition = Vector3((float)blockPosition.x, (float)height - .5f, (float)blockPosition.y);
	theRenderer->DrawCube(cursorPosition, Vector3(1.f, 0.01f, 1.f), color);

	theRenderer = nullptr;
}


void Map::SetActionableTilesForActor(Actor* actor, bool doesExcludeOccupiedTiles = true, int maxDistance = 1, int minDistance = 0)
{
	m_heatMap = new HeatMap(IntVector2(m_dimensions.x, m_dimensions.z), LARGE_FLOAT);
	m_heatMap->m_heatPerTile[GetBlockIndexByPositionXZ(actor->GetPositionInXZ())] = 0;

	bool isMapSolved = false;
	int iterations = 0;
	bool didAValueChange = false;

	int maxJumpHeight = actor->m_definition->m_maxJumpHeight;

	int tileIndex = 0;
	while(isMapSolved == false)
	{
		for(tileIndex = 0; tileIndex < (int)m_heatMap->m_heatPerTile.size(); tileIndex++)
		{
			if(m_heatMap->m_heatPerTile[tileIndex] < LARGE_INT)
			{
				IntVector2 position = GetBlockPositionByIndexXZ(tileIndex);

				std::vector<int> neighboringTiles;

				neighboringTiles.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x, position.y + 1)));
				neighboringTiles.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x, position.y - 1)));
				neighboringTiles.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x - 1, position.y)));
				neighboringTiles.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x + 1, position.y)));

				for (int currentNeighborIndex = 0; currentNeighborIndex < (int)neighboringTiles.size(); currentNeighborIndex++)
				{                     
					float currentTileValue = m_heatMap->m_heatPerTile[tileIndex];
					int neighborIndex = neighboringTiles[currentNeighborIndex];

					if (neighborIndex == -1)
					{
						continue;
					}

					TODO("If you introduce costs per block defintion or something, add this code back. (refer to pather assignment from S1)");
					//float neighborCost = m_m_heights[neighborIndex];
					bool isGreater = (float)m_heatMap->m_heatPerTile[neighborIndex] > (float)currentTileValue + 1.f;

					IntVector2 neighborIndexPosition = m_heatMap->GetPositionByIndex(neighborIndex);
					int heightDifference = (GetHeightForPositionXZ(neighborIndexPosition) - GetHeightForPositionXZ(position));

					bool isJumpable = (heightDifference <= maxJumpHeight);
					if (isGreater && isJumpable && currentTileValue < maxDistance)
					{
						m_heatMap->m_heatPerTile[neighborIndex] = currentTileValue + 1;
						didAValueChange = true;
					}
				}
			}
		}

		if (didAValueChange == false)
		{
			isMapSolved = true;
		}
		if(iterations == 999) //fail safe
		{
			isMapSolved = true;
		}
		iterations++;

		didAValueChange = false;
	}

	//add visibile tiles
	for(int tileIndex = 0; tileIndex < (int)m_heatMap->m_heatPerTile.size(); tileIndex++)
	{
		if(m_heatMap->m_heatPerTile[tileIndex] <= maxDistance && m_heatMap->m_heatPerTile[tileIndex] >= minDistance)
		{		
			if(doesExcludeOccupiedTiles)
			{
				bool isOccupied = false;
				for(int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
				{
					IntVector2 tilePosition = m_heatMap->GetPositionByIndex(tileIndex);
					if(m_actors[actorIndex]->GetPositionInXZ() == tilePosition)
					{
						isOccupied = true;
						break;
					}
				}

				if(isOccupied == false)
				{
					m_pathingDrawTilesIndices.push_back(tileIndex);		
				}
			}
			else
			{
				m_pathingDrawTilesIndices.push_back(tileIndex);			
			}			
		}		
	}
}

std::vector<int> Map::GetOrderedIndexesForShortestPathInCurrentHeatMap(IntVector2 endPosition)
{
	std::vector<int> orderedIndexes;

	int currentIndex = GetBlockIndexByPositionXZ(endPosition);

	orderedIndexes.push_back(currentIndex);

	bool isPathResolved = false;        
	while(!isPathResolved)
	{
		IntVector2 currentPosition = GetBlockPositionByIndexXZ( currentIndex);

		std::vector<int> neighboringTiles;

		int northTileIndex = GetBlockIndexByPositionXZ(IntVector2(currentPosition.x, currentPosition.y + 1));
		int southTileIndex = GetBlockIndexByPositionXZ(IntVector2(currentPosition.x, currentPosition.y - 1));
		int eastTileIndex = GetBlockIndexByPositionXZ(IntVector2(currentPosition.x + 1, currentPosition.y));
		int westTileIndex = GetBlockIndexByPositionXZ(IntVector2(currentPosition.x - 1, currentPosition.y));

		neighboringTiles.push_back(northTileIndex);
		neighboringTiles.push_back(southTileIndex);
		neighboringTiles.push_back(eastTileIndex);
		neighboringTiles.push_back(westTileIndex);

		float lowestMapValue = m_heatMap->m_heatPerTile[currentIndex];
		int newIndex = currentIndex;
		for(int i = 0; i < (int)neighboringTiles.size(); i++)
		{
			if (neighboringTiles[i] == -1)
			{
				continue;
			}

			float neighborHeat = m_heatMap->m_heatPerTile[neighboringTiles[i]];
			if (neighborHeat < lowestMapValue)
			{
				lowestMapValue = neighborHeat;
				newIndex = neighboringTiles[i];
			}
		}

		if(lowestMapValue == m_heatMap->m_heatPerTile[currentIndex] || lowestMapValue == 0)
		{
			isPathResolved = true;
		}

		currentIndex = newIndex;

		orderedIndexes.push_back(newIndex);
	}      

	return orderedIndexes; 


	return orderedIndexes;
}

void Map::ResetActionableTiles()
{
	m_pathingDrawTilesIndices.clear();
	m_pathingDrawTilesIndices.resize(0);

	delete(m_heatMap);
	m_heatMap = nullptr;
}

void Map::UpdateIsCursorPositionInPathingIndices()
{
	int cursorIndexXZ = GetBlockIndexByPositionXZ(m_cursorPosition);
	for(int tileIndex = 0; tileIndex < (int)m_pathingDrawTilesIndices.size(); tileIndex++)
	{
		if(m_pathingDrawTilesIndices[tileIndex] == cursorIndexXZ)
		{
			m_isCursorPositionInPathingIndices = true;
			return;
		}
	}

	m_isCursorPositionInPathingIndices = false;
}

void Map::UpdateAreaOfEffectTiles(AbilityStruct ability)
{
	ResetAreaOfEffectTiles();
	std::vector<int> tileIndicesInAOE;
	if(m_isCursorPositionInPathingIndices)
	{
		int aoeRangeMin = ability.m_areaOfEffectHorizontal.min;
		int aoeRangeMax = ability.m_areaOfEffectHorizontal.max;

		IntVector2 position = m_cursorPosition;
		m_areaOfEffectCenterIndex = GetBlockIndexByPositionXZ(m_cursorPosition);
		//tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(m_cursorPosition));

		//get manhattan distance
		for(int range = aoeRangeMin; range <= aoeRangeMax; range++)
		{
			if(range > 0)
			{
				//get north, south, east, west tiles
				tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x, position.y + range)));
				tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x, position.y - range)));
				tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x - range, position.y)));
				tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x + range, position.y)));

				if(range > 1)
				{
					////get northeast, southeast, northwest, southwest
					tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x + (range - 1), position.y + (range - 1))));
					tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x + (range - 1), position.y - (range - 1))));
					tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x - (range - 1), position.y + (range - 1))));
					tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x - (range - 1), position.y - (range - 1))));
				}
			}
			if(range == 0)
			{
				tileIndicesInAOE.push_back(GetBlockIndexByPositionXZ(IntVector2(position.x, position.y)));
			}
		}		
		int heightAtCursorPosition = GetHeightForPositionXZ(m_cursorPosition);
		int minHeightDifference = ability.m_areaOfEffectVertical.min;
		int maxHeightDifference = ability.m_areaOfEffectVertical.max;

		for(int tileIndex = 0; tileIndex < (int)tileIndicesInAOE.size(); tileIndex++)
		{
			if(tileIndicesInAOE[tileIndex] < (m_dimensions.x * m_dimensions.z) && tileIndicesInAOE[tileIndex] > 0)
			{
				IntVector2 tilePosition = GetBlockPositionByIndexXZ(tileIndicesInAOE[tileIndex]);
				int tileHeight = GetHeightForPositionXZ(tilePosition);

				bool isInVerticalRange = true;
				if(tileHeight > heightAtCursorPosition)
				{
					if(tileHeight - heightAtCursorPosition > maxHeightDifference)
					{
						isInVerticalRange = false;
					}
				}
				if(tileHeight < heightAtCursorPosition)
				{
					if(heightAtCursorPosition - tileHeight > (-1 * minHeightDifference))
					{
						isInVerticalRange = false;
					}
				}

				if(isInVerticalRange == true)
				{
					m_areaOfEffectDrawTilesIndices.push_back(tileIndicesInAOE[tileIndex]);
				}
			}
		}
	}
	else
	{
		ResetAreaOfEffectTiles();
	}
}

bool Map::GetIsPositionInsideAreaOfEffectTiles(IntVector2 position)
{
	int positionIndex = GetBlockIndexByPositionXZ(position);

	for(int searchIndex = 0; searchIndex < (int)m_areaOfEffectDrawTilesIndices.size(); searchIndex++)
	{
		if(positionIndex == m_areaOfEffectDrawTilesIndices[searchIndex])
		{
			return true;
		}
	}

	return false;
}

void Map::ResetAreaOfEffectTiles()
{
	m_areaOfEffectCenterIndex = -1;
	m_areaOfEffectDrawTilesIndices.clear();
	m_areaOfEffectDrawTilesIndices.resize(0);
}

Actor* Map::GetActorAtCursorPosition()
{
	for(int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++)
	{
		if(m_actors[actorIndex]->GetPositionInXZ() == m_cursorPosition)
		{
			return m_actors[actorIndex];
		}
	}

	return nullptr;
}
