
//#include "Engine\Utility\Grid.hpp"
//
//
///* Example
// y|||||||||||
//x|0  1  2  3|
// |4  5  6  7|
// |8  9 10 11|
// ||||||||||||
//*/
//
//Grid::Grid()
//{
//}
//
//Grid::Grid(int numRows, int numColumns)
//{
//	m_dimensions.x = numRows;
//	m_dimensions.y = numColumns;
//
//	m_gridData = (int*)malloc(sizeof(int) * m_dimensions.x * m_dimensions.y);
//}
//
//Grid::Grid(const IntVector2& dimensions)
//{
//	m_dimensions.x = dimensions.x;
//	m_dimensions.y = dimensions.y;
//
//	m_gridData = (int*)malloc(sizeof(int) * m_dimensions.x * m_dimensions.y);
//}
//
//Grid::~Grid()
//{
//	free((void*)m_gridData);
//	m_gridData = nullptr;
//}
//
//void Grid::InitializeGrid(int numRows, int numColumns)
//{
//	if (m_gridData != nullptr)
//	{
//		free((void*)m_gridData);
//		m_gridData = nullptr;
//	}
//
//	m_dimensions.x = numRows;
//	m_dimensions.y = numColumns;
//
//	m_gridData = (int*)malloc(sizeof(int) * m_dimensions.x * m_dimensions.y);
//}
//
//void Grid::IntitializeGrid(const IntVector2& dimensions)
//{
//	if (m_gridData != nullptr)
//	{
//		free((void*)m_gridData);
//		m_gridData = nullptr;
//	}
//
//	m_dimensions.x = dimensions.x;
//	m_dimensions.y = dimensions.y;
//
//	m_gridData = (int*)malloc(sizeof(int) * m_dimensions.x * m_dimensions.y);
//}
//
//void Grid::ResetGridData()
//{
//	for (int cellIndex = 0; cellIndex < m_dimensions.x * m_dimensions.y; ++cellIndex)
//	{
//		m_gridData[cellIndex] = 0;
//	}		 
//}
//
//int Grid::GetValueAtCoordinate(int x, int y)
//{
//	int value = m_gridData[x + (y * m_dimensions.x)];
//}
//
//int Grid::GetValueAtCoordinate(const IntVector2& coordinate)
//{
//	int value = m_gridData[coordinate.x + (coordinate.y * m_dimensions.x)];
//}
//
//int Grid::GetValueAtIndex(int index)
//{
//	int x = index % m_dimensions.x;
//	int y = index / m_dimensions.x;
//
//	return GetValueAtCoordinate(x, y);
//}
//
//int* Grid::GetRow(int x)
//{
//	return nullptr;
//}
//
//int* Grid::GetColumn(int y)
//{
//	return nullptr;
//}
//
//bool Grid::IsCellValid(int x, int y)
//{
//	if(x >= 0 && x < m_dimensions.x && y >= 0 && y < m_dimensions.y)
//		return true;
//	else
//		return false;
//}
//
//int Grid::IsCellValid(const IntVector2& cellCoordinate)
//{
//	if (cellCoordinate.x >= 0 && cellCoordinate.x < m_dimensions.x && cellCoordinate.y >= 0 && cellCoordinate.y < m_dimensions.y)
//		return true;
//	else
//		return false;
//}
