#pragma once
#include <vector>
#include "Engine/Math/IntVector2.hpp"

class HeatMap
{
public:
	HeatMap(IntVector2 dimensions, float initialHeat);
	void SetHeat(const IntVector2& cellCoords, float heatValue);
	float GetHeat(const IntVector2& cellCoords);
	void AddHeat(const IntVector2& cellCoords, float heatValue);
	IntVector2 GetPositionByIndex(const int & index);
public:
	std::vector<int> m_heatPerTile;
	IntVector2 m_dimensions;
};
