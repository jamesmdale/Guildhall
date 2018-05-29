#include "Game/HeatMap.hpp"

HeatMap::HeatMap(IntVector2 dimensions, float initialHeat)
{
	m_dimensions = dimensions;
	
	int numCells = m_dimensions.x * m_dimensions.y;

	for(int cellIndex = 0; cellIndex < numCells; cellIndex++)
	{
		m_heatPerGridCell[cellIndex] = initialHeat;
	}
}

void HeatMap::SetHeat(const IntVector2& cellCoords, float heatValue)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerGridCell.size())
	{
		m_heatPerGridCell[cellIndex] = heatValue;
	}
}

float HeatMap::GetHeat(const IntVector2& cellCoords)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerGridCell.size())
	{
		return m_heatPerGridCell[cellIndex];
	}
	else
	{
		return -1;
	}
}

void HeatMap::AddHeat(const IntVector2& cellCoords, float heatValue)
{
	int cellIndex = cellCoords.x * cellCoords.y;

	if(cellIndex <= (int)m_heatPerGridCell.size())
	{
		m_heatPerGridCell[cellIndex] += heatValue;
	}
}