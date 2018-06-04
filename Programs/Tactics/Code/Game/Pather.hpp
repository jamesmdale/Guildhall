#pragma once
#include <vector>

class Pather
{
	public:
		std::vector<int> GetVisitableTiles(int x, int y, float maxCost);
		

	public:
		std::vector<int> m_Costs;
		int m_width;
		int m_height;

};