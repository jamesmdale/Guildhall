#pragma once
#include <vector>
#include "Engine\Math\IntVector2.hpp"

class DistanceField
{
public:
	DistanceField(int width, int height, std::vector<int> costs);

	int GetIndex(int x, int y);
	IntVector2 GetLocation(int index);
	void Seed(int x, int y);
	void SolveTo(float maxDist);
	float GetDistance(int x, int y);
	std::vector<int> GetPath(int endX, int endY);

public:
	int m_width;
	int m_height; 
	std::vector<int> m_costs;
	std::vector<float> m_heatMap;
};