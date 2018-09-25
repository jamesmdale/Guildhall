#include "Game\Map\Map.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\MapGenStep.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"
#include "Game\PointOfInterest.hpp"

//  =========================================================================================
Map::Map(MapDefinition* definition, const std::string & mapName, RenderScene2D* renderScene)
{
	m_name = mapName;
	m_mapDefinition = definition;

	int numTilesX = GetRandomIntInRange(m_mapDefinition->m_width.min, m_mapDefinition->m_width.max);
	int numTilesY = GetRandomIntInRange(m_mapDefinition->m_height.min, m_mapDefinition->m_height.max);

	m_dimensions = IntVector2(numTilesX, numTilesY);

	for (int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
	{
		for (int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
		{		
			Tile* newTile = new Tile();

			newTile->m_tileCoords = IntVector2(xCoordinate, yCoordinate);
			newTile->m_tileDefinition = TileDefinition::s_tileDefinitions[m_mapDefinition->m_defaultTile->m_name];

			newTile->Initialize();

			m_tiles.push_back(newTile);
			newTile = nullptr;
		}
	}

	for (int genStepsIndex = 0; genStepsIndex < (int)m_mapDefinition->m_genSteps.size(); genStepsIndex++)
	{
		int iterations = GetRandomIntInRange(m_mapDefinition->m_iterations.min, m_mapDefinition->m_iterations.max);
		float chanceToRun = GetRandomFloatZeroToOne();
		if (chanceToRun <= m_mapDefinition->m_chanceToRun)
		{
			for (int iterationIndex = 0; iterationIndex < iterations; iterationIndex++)
			{
				m_mapDefinition->m_genSteps[genStepsIndex]->Run(*this);
			}
		}
	}

	m_mapWorldBounds = AABB2(0.f, 0.f, m_dimensions.x * g_tileSize, m_dimensions.y * g_tileSize);
}

//  =========================================================================================
Map::~Map()
{
	m_mapDefinition = nullptr;
}

//  =========================================================================================
void Map::Render()
{
}

//  =========================================================================================
void Map::Update(float timeDelta)
{
}

//  =========================================================================================
void Map::DeleteDeadEntities()
{
}

//  =========================================================================================
void Map::UpdatePlayerInput()
{
}

//  =========================================================================================
int Map::GetActorIndex()
{
	return 0;
}

//  =========================================================================================
Grid<int>* Map::GetAsGrid()
{
	Grid<int>* grid = new Grid<int>();

	grid->InitializeGrid(0, m_dimensions.x, m_dimensions.y);

	for (int tileIndex = 0; tileIndex < (int)m_tiles.size(); tileIndex++)
	{
		int value = m_tiles[tileIndex]->m_tileDefinition->m_allowsWalking ? 0 : 1;

		if(value != 0)
			grid->SetValueAtIndex(value, tileIndex);
	}

	return grid;
}

//  =========================================================================================
bool Map::IsTileBlockingAtCoordinate(const IntVector2 & coordinate)
{
	bool isBlocking = !GetTileAtCoordinate(coordinate)->m_tileDefinition->m_allowsWalking;
	return isBlocking;	
}

//  =========================================================================================
Tile* Map::GetTileAtCoordinate(const IntVector2& coordinate)
{
	return m_tiles[coordinate.x + (coordinate.y * m_dimensions.x)];
}

//  =========================================================================================
PointOfInterest* Map::GeneratePointOfInterest(ePointOfInterestType poiType)
{
	//determine if random location is unblocked
	bool isLocationValid = false;
	IntVector2 randomLocation = IntVector2::ONE;
	IntVector2 accessCoordinate = IntVector2::ONE;

	while (!isLocationValid)
	{
		randomLocation = GetRandomNonBlockedCoordinateInMapBounds();
		
		//check tile to north, to the right, and to the northeast to see if they are even good
		if(CheckIsCoordianteValid(IntVector2(randomLocation.x + 1, randomLocation.y)) 
			&& CheckIsCoordianteValid(IntVector2(randomLocation.x, randomLocation.y + 1))
			&& CheckIsCoordianteValid(IntVector2(randomLocation.x + 1, randomLocation.y + 1)))
		{
			//check tile to north, to the right, and to the northeast to see if they are blocked
			if (!IsTileBlockingAtCoordinate(IntVector2(randomLocation.x + 1, randomLocation.y))
				&& !IsTileBlockingAtCoordinate(IntVector2(randomLocation.x, randomLocation.y + 1))
				&& !IsTileBlockingAtCoordinate(IntVector2(randomLocation.x + 1, randomLocation.y + 1)))
			{
				/*now we should get a random location for our access location for the poi
				there are potentially eight acceptable locations
				   [5][4]
				[6][X][X][3]
				[7][X][X][2]
				   [0][1]
				*/

				bool isAccessLocationValid =  false;
				int accessIterationAttemptCount = 0;

				while (!isAccessLocationValid && accessIterationAttemptCount < 7)
				{
					int randomAccessLocation = GetRandomIntInRange(0, 7);
					switch (randomAccessLocation)
					{
					case 0:
						accessCoordinate = IntVector2(randomLocation.x, randomLocation.y - 1);
						break;
					case 1:
						accessCoordinate = IntVector2(randomLocation.x + 2, randomLocation.y - 1);
						break;
					case 2:
						accessCoordinate = IntVector2(randomLocation.x + 2, randomLocation.y);
						break;
					case 3:
						accessCoordinate = IntVector2(randomLocation.x + 2, randomLocation.y + 1);
						break;
					case 4:
						accessCoordinate = IntVector2(randomLocation.x + 1, randomLocation.y + 2);
						break;
					case 5:
						accessCoordinate = IntVector2(randomLocation.x, randomLocation.y + 2);
						break;
					case 6:
						accessCoordinate = IntVector2(randomLocation.x - 1, randomLocation.y + 1);
						break;
					case 7:
						accessCoordinate = IntVector2(randomLocation.x - 1, randomLocation.y);
						break;
					}

					if (CheckIsCoordianteValid(accessCoordinate) && !IsTileBlockingAtCoordinate(accessCoordinate))
					{
						isAccessLocationValid = true;
						isLocationValid = true;
					}

					if (!isAccessLocationValid)				
						++accessIterationAttemptCount;					
				}

				if(isAccessLocationValid)
					isLocationValid = true;
			}
		}		
	}

	//Location is valid, therefore we can replace tiles with building tiles

	//starting tile
	Tile* tile = GetTileAtCoordinate(randomLocation);
	tile->m_tileDefinition = TileDefinition::s_tileDefinitions.find("Building")->second;

	//tile to east
	tile = GetTileAtCoordinate(IntVector2(randomLocation.x + 1, randomLocation.y));
	tile->m_tileDefinition = TileDefinition::s_tileDefinitions.find("Building")->second;

	//tile to northeast
	tile = GetTileAtCoordinate(IntVector2(randomLocation.x + 1, randomLocation.y + 1));
	tile->m_tileDefinition = TileDefinition::s_tileDefinitions.find("Building")->second;

	//tile to north
	tile = GetTileAtCoordinate(IntVector2(randomLocation.x, randomLocation.y + 1));
	tile->m_tileDefinition = TileDefinition::s_tileDefinitions.find("Building")->second;

	//building access tile
	tile = GetTileAtCoordinate(accessCoordinate);
	tile->m_tileDefinition = TileDefinition::s_tileDefinitions.find("BuildingAccess")->second;

	PointOfInterest* poi = new PointOfInterest(poiType, randomLocation, accessCoordinate);

	//cleanup
	tile = nullptr;

	return poi;
}

//  =========================================================================================
IntVector2 Map::GetTileCoordinateOfPosition(const Vector2& position)
{
	float clientWidth = Window::GetInstance()->GetClientWidth();

	Vector2 positionInCoordinateSpace = position * g_divideTileSize;
	IntVector2 tileCoordinate = IntVector2(RoundToNearestInt(positionInCoordinateSpace.x), RoundToNearestInt(positionInCoordinateSpace.y));
	return tileCoordinate;
}

//  =========================================================================================
Vector2 Map::GetWorldPositionOfMapCoordinate(const IntVector2& position)
{
	Vector2 worldPosition = m_tiles[position.x + (position.y * m_dimensions.x)]->GetWorldSpaceCoordinates();
	return worldPosition;
}

//  =========================================================================================
bool Map::CheckIsPositionValid(const Vector2& position)
{
	IntVector2 tileCoordinate = GetTileCoordinateOfPosition(position);
	return CheckIsCoordianteValid(tileCoordinate);
}

//  =========================================================================================
bool Map::CheckIsCoordianteValid(const IntVector2 & coordinate)
{
	bool isCoordinateValid = false;

	if(coordinate.x >= 0 && coordinate.x < m_dimensions.x && coordinate.y >= 0 && coordinate.y < m_dimensions.y)
		isCoordinateValid = true;

	return isCoordinateValid;
}

//  =========================================================================================
Vector2 Map::GetRandomNonBlockedPositionInMapBounds()
{
	bool isNonBlocked = false;
	Vector2 randomPoint = Vector2::NEGATIVE_ONE;

	while (isNonBlocked == false)
	{
		randomPoint = m_mapWorldBounds.GetRandomPointInBounds();
		IntVector2 correspondingTileCoordinate = GetTileCoordinateOfPosition(randomPoint);

		if (m_tiles[correspondingTileCoordinate.x + (correspondingTileCoordinate.y * m_dimensions.x)]->m_tileDefinition->m_allowsWalking)
		{
			isNonBlocked = true;
		}
	}

	return randomPoint;
}

//  =========================================================================================
IntVector2 Map::GetRandomNonBlockedCoordinateInMapBounds()
{
	bool isBlocked = true;
	IntVector2 randomPoint;

	while (isBlocked)
	{
		randomPoint = IntVector2(GetRandomIntInRange(0, m_dimensions.x - 1), GetRandomIntInRange(0, m_dimensions.y - 1));
		isBlocked = IsTileBlockingAtCoordinate(randomPoint);
	}

	return randomPoint;
}
