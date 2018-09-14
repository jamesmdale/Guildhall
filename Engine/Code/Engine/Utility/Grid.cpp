#include <stdlib.h>
#include "Engine\Utility\Grid.hpp"


/* Example
 y|||||||||||
x|0  1  2  3|
 |4  5  6  7|
 |8  9 10 11|
 ||||||||||||
*/

Grid::Grid()
{
}

Grid::Grid(int numRows, int numColumns)
{
	m_numRows = numRows;
	m_numColumns - numColumns;

	m_gridData = (int*)malloc(sizeof(int) * m_numRows * m_numColumns);
}

Grid::~Grid()
{
	free((void*)m_gridData);
	m_gridData = nullptr;
}

void Grid::InitializeGrid(int numRows, int numColumns)
{
	if (m_gridData != nullptr)
	{
		free((void*)m_gridData);
		m_gridData = nullptr;
	}

	m_numRows = numRows;
	m_numColumns - numColumns;

	m_gridData = (int*)malloc(sizeof(int) * m_numRows * m_numColumns);
}

void Grid::ResetGridData()
{
	for (int cellIndex = 0; cellIndex < m_numRows * m_numColumns; ++cellIndex)
	{
		m_gridData[cellIndex] = 0;
	}		 
}

int Grid::GetValueAtCoordinate(int x, int y)
{
	int value = m_gridData[x + (y * m_numRows)];
}

int Grid::GetValueAtIndex(int index)
{
	int x = index % m_numRows;
	int y = index / m_numRows;

	return GetValueAtCoordinate(x, y);
}

int* Grid::GetRow(int x)
{
	return nullptr;
}

int* Grid::GetColumn(int y)
{
	return nullptr;
}

bool Grid::IsCellValid(int x, int y)
{
	if(x >= 0 && x < m_numRows && y >= 0 && y < m_numColumns)
		return true;
	else
		return false;
}
