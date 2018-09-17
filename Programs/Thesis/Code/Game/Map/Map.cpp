#include "Game\Map\Map.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\MapGenStep.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"

//  =========================================================================================
Map::Map(MapDefinition* definition, const std::string & mapName, RenderScene2D* renderScene)
{
	m_name = mapName;
	m_mapDefinition = definition;
	m_renderScene = renderScene;

	int numTilesX = GetRandomIntInRange(m_mapDefinition->m_width.min, m_mapDefinition->m_width.max);
	int numTilesY = GetRandomIntInRange(m_mapDefinition->m_height.min, m_mapDefinition->m_height.max);

	m_dimensions = IntVector2(numTilesX, numTilesY);

	for (int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
	{
		for (int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
		{		
			Tile* newTile = new Tile();

			newTile->m_tileCoords = IntVector2(xCoordinate, yCoordinate);
			newTile->m_tileDefinition = TileDefinition::s_definitions[m_mapDefinition->m_defaultTile->m_name];
			newTile->m_renderScene = m_renderScene;

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

	float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;

	m_mapBounds = AABB2(0.f, 0.f, m_dimensions.x * dimensions, m_dimensions.y * dimensions);
}

//  =========================================================================================
Map::~Map()
{
	m_renderScene = nullptr;
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
IntVector2 Map::GetTileCoordinateOfPosition(Vector2 position)
{
	return IntVector2(RoundToNearestInt(position.x), RoundToNearestInt(position.y));
}

Vector2 Map::GetWorldPositionOfMapCoordinate(Vector2 position)
{
	IntVector2 position = GetTileCoordinateOfPosition(
}
