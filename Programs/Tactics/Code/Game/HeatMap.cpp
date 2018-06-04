#include "Game/HeatMap.hpp"

HeatMap::HeatMap(IntVector2 dimensions, float initialHeat)
{
	m_dimensions = dimensions;

	int numCells = m_dimensions.x * m_dimensions.y;
	m_heatPerTile.resize(numCells);

	for(int cellIndex = 0; cellIndex < numCells; cellIndex++)
	{
		m_heatPerTile[cellIndex] = initialHeat;
	}
}

void HeatMap::SetHeat(const IntVector2& cellCoords, float heatValue)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerTile.size())
	{
		m_heatPerTile[cellIndex] = heatValue;
	}
}

float HeatMap::GetHeat(const IntVector2& cellCoords)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerTile.size())
	{
		return m_heatPerTile[cellIndex];
	}
	else
	{
		return -1;
	}
}

void HeatMap::AddHeat(const IntVector2& cellCoords, float heatValue)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerTile.size())
	{
		m_heatPerTile[cellIndex] += heatValue;
	}
}

IntVector2 HeatMap::GetPositionByIndex(const int& index)
{
	int z = index / m_dimensions.x; 
	int x = index % m_dimensions.x; 

	return IntVector2(x, z);
}