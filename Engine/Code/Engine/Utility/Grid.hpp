#pragma once
struct GridCell
{
	int rowIndex = 0;
	int columnIndex = 0;

	int totalCost = 0;
	int movementCost = 0;
	int heuristicCost = 0;
};


class Grid
{
public:
	Grid();
	Grid(int numRows, int numColumns);
	~Grid();

	//allocates new data set based on new Rows and Columns (NOTE: deletes existing grid data if exists)
	void InitializeGrid(int numRows, int numColumns);

	//clears all grid data and sets to all 0s
	void ResetGridData();

	//helper functions
	int GetValueAtCoordinate(int x, int  y);
	int GetValueAtIndex(int index);
	int* GetRow(int x);
	int* GetColumn(int y);
	bool IsCellValid(int x, int y);

public:
	int m_numRows = 0;
	int m_numColumns = 0;

	//2 dimensional array of ints
	int* m_gridData = nullptr;
};

