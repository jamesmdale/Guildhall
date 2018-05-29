#include "DistanceField.hpp"
#include "Game/GameCommon.hpp"

DistanceField::DistanceField(int width, int height, std::vector<int> costs)
{
	m_width = width;
	m_height = height;
	m_costs = costs;

	int dimensions = width * height;
	for(int i = 0; i < dimensions; i++)
	{
		m_heatMap.push_back(LARGE_FLOAT);
	}
}

int DistanceField::GetIndex( int x, int y )
{
	if ((x < 0) || (x >= m_width) || (y < 0) || (y >= m_height)) {
		return -1; 
	} else {
		return y * m_width + x; 
	}
}

//----------------------------------------------------------------------------------
// Get a position from an index; 
IntVector2 DistanceField::GetLocation(int index)
{
	int y = index / m_width; 
	int x = index % m_width; 

	return IntVector2(x, y);
} 

//----------------------------------------------------------------------------------
// Set the initial seed points [places where a path can start from]
void DistanceField::Seed( int x, int y )
{
	m_heatMap[GetIndex(x, y)] = 0;        
}

//----------------------------------------------------------------------------------
// Solve the distance field up to the provided max distance. 
void DistanceField::SolveTo( float maxDist )
{
	bool isMapSolved = false;
	int iterations = 0;
	bool didAValueChange = false;
	while (!isMapSolved && iterations < 9999)
	{
		iterations++;
		didAValueChange = false;
		for (int mapIndex = 0; mapIndex < (int)m_heatMap.size(); mapIndex++)
		{
			if (m_heatMap[mapIndex] < LARGE_FLOAT)
			{
				IntVector2 position = GetLocation(mapIndex);

				std::vector<int> neighboringTiles;

				int northTileIndex = GetIndex(position.x, position.y + 1);
				int southTileIndex = GetIndex(position.x, position.y - 1);
				int eastTileIndex = GetIndex(position.x + 1, position.y);
				int westTileIndex = GetIndex(position.x - 1, position.y);

				neighboringTiles.push_back(northTileIndex);
				neighboringTiles.push_back(southTileIndex);
				neighboringTiles.push_back(eastTileIndex);
				neighboringTiles.push_back(westTileIndex);

				for (int currentNeighborIndex = 0; currentNeighborIndex < (int)neighboringTiles.size(); currentNeighborIndex++)
				{                     
					float currentTileValue = m_heatMap[mapIndex];
					int neighborIndex = neighboringTiles[currentNeighborIndex];

					if (neighborIndex == -1)
					{
						continue;
					}

					float neighborCost = m_costs[neighborIndex];
					bool isGreater = (m_heatMap[neighborIndex] - 0.01f) > (currentTileValue + neighborCost);
					if (isGreater)// && (currentTileValue + neighborCost) < maxDist)
					{
						m_heatMap[neighborIndex] = currentTileValue + neighborCost;
						float currentNeighborValue = m_heatMap[neighborIndex];
						didAValueChange = true;
					}
				}
			}
		}

		if (didAValueChange == false)
		{
			isMapSolved = true;
		}
		if(iterations == 9998)
		{
			int h = 0;
		}
	}
}
//----------------------------------------------------------------------------------
// Get how far a point is from a seed point along it's path
float DistanceField::GetDistance( int x, int y )
{
	return m_heatMap[GetIndex(x, y)];
}

//----------------------------------------------------------------------------------
// Given a point - give the path back to the closest seed point inclusive.
// If the point given is a seed point, return a single point
// If the point is outside of the solve distance, return an empty list.
std::vector<int> DistanceField::GetPath( int endX, int endY )
{
	std::vector<int> path;

	int currentIndex = GetIndex(endX, endY);

	int tileX = endX;
	int tileY = endY;

	path.push_back(currentIndex);
	bool isPathResolved = false;        
	while(!isPathResolved)
	{
		IntVector2 position = GetLocation(currentIndex);

		std::vector<int> neighboringTiles;

		int northTileIndex = GetIndex(position.x, position.y + 1);
		int southTileIndex = GetIndex(position.x, position.y - 1);
		int eastTileIndex = GetIndex(position.x + 1, position.y);
		int westTileIndex = GetIndex(position.x - 1, position.y);


		neighboringTiles.push_back(northTileIndex);
		neighboringTiles.push_back(southTileIndex);
		neighboringTiles.push_back(eastTileIndex);
		neighboringTiles.push_back(westTileIndex);

		float lowestMapValue = m_heatMap[currentIndex];
		int newIndex = currentIndex;
		for(int neighborIndex = 0; neighborIndex < (int)neighboringTiles.size(); neighborIndex++)
		{
			if (neighboringTiles[neighborIndex] == -1)
			{
				continue;
			}

			float neighborHeat = m_heatMap[neighboringTiles[neighborIndex]];
			if (neighborHeat < lowestMapValue)
			{
				lowestMapValue = neighborHeat;
				newIndex = neighboringTiles[neighborIndex];
			}
		}

		if(lowestMapValue == m_heatMap[currentIndex] || lowestMapValue == 0)
		{
			isPathResolved = true;
		}

		currentIndex = newIndex;

		path.push_back(newIndex);
	}      

	return path; 
}
