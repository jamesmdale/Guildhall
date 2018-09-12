#include "Game\Map\Map.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Game\Map\MapGenStep.hpp"

//  =========================================================================================
Map::Map(MapDefinition* definition, const std::string & mapName, RenderScene2D* renderScene)
{
	m_name = mapName;
	m_mapDefinition = definition;
	m_renderScene = renderScene;

	int numTilesX = GetRandomIntInRange(m_mapDefinition->m_width.min, m_mapDefinition->m_width.max);
	int numTilesY = GetRandomIntInRange(m_mapDefinition->m_height.min, m_mapDefinition->m_height.max);

	m_dimensions = IntVector2(numTilesX, numTilesY);

	for (int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
	{
		for (int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
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

IntVector2 Map::GetTileCoordinateOfPosition(Vector2 position)
{
	return IntVector2(RoundToNearestInt(position.x), RoundToNearestInt(position.y));
}