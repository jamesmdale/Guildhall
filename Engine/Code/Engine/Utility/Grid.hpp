#pragma once
#include <stdlib.h>
#include "Engine\Math\IntVector2.hpp"

template <class T>
class Grid
{
public:
	// constructors =========================================================================================
	Grid()
	{

	}

	Grid(const T& defaultValue, int numRows, int numColumns)
	{
		m_dimensions.x = numRows;
		m_dimensions.y = numColumns;

		m_gridData = (T*)malloc(sizeof(T) * m_dimensions.x * m_dimensions.y);
	
		for (int dataIndex = 0; dataIndex < (int)m_dimensions.x * m_dimensions.y; ++dataIndex)
		{
			SetValueAtIndex(defaultValue, dataIndex);
		}

		//memset(m_gridData, defaultValue, sizeof(T) * m_dimensions.x * m_dimensions.y);
	}

	Grid(const T& defaultValue, const IntVector2& dimensions)
	{
		m_dimensions.x = dimensions.x;
		m_dimensions.y = dimensions.y;

		m_gridData = (T*)malloc(sizeof(T) * m_dimensions.x * m_dimensions.y);
		
		for (int dataIndex = 0; dataIndex < (int)m_dimensions.x * m_dimensions.y; ++dataIndex)
		{
			SetValueAtIndex(defaultValue, dataIndex);
		}

		//memset(m_gridData, defaultValue, sizeof(T) * m_dimensions.x * m_dimensions.y);
	}

	virtual ~Grid()
	{
		free((void*)m_gridData);
		m_gridData = nullptr;
	}

	//allocates new data set based on new Rows and Columns (NOTE: deletes existing grid data if exists)
	void InitializeGrid(const T& defaultValue, int numRows, int numColumns)
	{
		if (m_gridData != nullptr)
		{
			free((void*)m_gridData);
			m_gridData = nullptr;
		}

		m_dimensions.x = numRows;
		m_dimensions.y = numColumns;

		m_gridData = (T*)malloc(sizeof(T) * m_dimensions.x * m_dimensions.y);

		for (int dataIndex = 0; dataIndex < (int)m_dimensions.x * m_dimensions.y; ++dataIndex)
		{
			SetValueAtIndex(defaultValue, dataIndex);
		}

		//memset(m_gridData, defaultValue, sizeof(T) * m_dimensions.x * m_dimensions.y);
	}

	void IntitializeGrid(const T& defaultValue, const IntVector2& dimensions)
	{
		if (m_gridData != nullptr)
		{
			free((void*)m_gridData);
			m_gridData = nullptr;
		}

		m_dimensions.x = dimensions.x;
		m_dimensions.y = dimensions.y;

		m_gridData = (T*)malloc(sizeof(T) * m_dimensions.x * m_dimensions.y);

		for (int dataIndex = 0; dataIndex < (int)m_dimensions.x * m_dimensions.y; ++dataIndex)
		{
			SetValueAtIndex(defaultValue, dataIndex);
		}

		//memset(m_gridData, defaultValue, sizeof(T) * m_dimensions.x * m_dimensions.y);
	}

	void ResetGridData(const T& defaultValue)
	{
		for (int cellIndex = 0; cellIndex < m_dimensions.x * m_dimensions.y; ++cellIndex)
		{
			m_gridData[cellIndex] = defaultValue;
		}
	}

	// getters =========================================================================================
	T GetValueAtCoordinate(int x, int  y)
	{
		T val = m_gridData[x + (y * m_dimensions.x)];
		return val;
	}

	T GetValueAtCoordinate(const IntVector2& coordinate)
	{
		T val =  m_gridData[coordinate.x + (coordinate.y * m_dimensions.x)];
		return val;
	}

	T* GetReferenceAtCoordinate(int x, int y)
	{
		T* val = &m_gridData[x + (y * m_dimensions.x)]
		return val;
	}

	T* GetReferenceAtCoordinate(const IntVector2& coordinate)
	{	
		T* val = &m_gridData[coordinate.x + (coordinate.y * m_dimensions.x)];
		return val;
	}

	T GetValueAtIndex(int index)
	{
		int x = index % m_dimensions.x;
		int y = index / m_dimensions.x;

		return GetValueAtCoordinate(x, y);
	}


	int GetIndexAtLocation(int x, int y)
	{
		if ((x < 0) || (x >= m_dimensions.x) || (y < 0) || (y >= m_dimensions.y)) 
		{
			return -1;
		}
		else 
		{
			return x + (y * m_dimensions.x);
		}
	}

	int GetIndexAtLocation(const IntVector2& coordinate)
	{
		if ((coordinate.x < 0) || (coordinate.x >= m_dimensions.x) || (coordinate.y < 0) || (coordinate.y >= m_dimensions.y))
		{
			return -1;
		}
		else
		{
			return coordinate.x + (coordinate.y * m_dimensions.x);
		}
	}

	// setters =========================================================================================
	void SetValueAtCoordinate(const T& value, int x, int y)
	{
		m_gridData[x + (y * m_dimensions.x)] = value;
	}

	void SetValueAtCoordinate(const T& value, IntVector2& coordinate)
	{
		m_gridData[coordinate.x + (coordinate.y * m_dimensions.x)] = value;
	}

	void SetValueAtIndex(const T& value, int index)
	{
		int x = index % m_dimensions.x;
		int y = index / m_dimensions.x;

		return SetValueAtCoordinate(value, x, y);
	}

	// validty checks =========================================================================================
	bool IsCellValid(int x, int y)
	{
		if (x >= 0 && x < m_dimensions.x && y >= 0 && y < m_dimensions.y)
			return true;
		else
			return false;
	}

	int IsCellValid(const IntVector2& cellCoordinate)
	{
		if (cellCoordinate.x >= 0 && cellCoordinate.x < m_dimensions.x && cellCoordinate.y >= 0 && cellCoordinate.y < m_dimensions.y)
			return true;
		else
			return false;
	}

	
public:
	IntVector2 m_dimensions;

	//2 dimensional array of type T
	T* m_gridData = nullptr;
};

