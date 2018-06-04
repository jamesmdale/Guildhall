#include "Pather.hpp"

std::vector<int> Pather::GetVisitableTiles(int x, int y, float maxCost)
{
	std::vector<int> visitableTiles;

	int count = Width * Height; 
	float[] distances = new float[count];
	int[] previous = new int[count];
	for (int i = 0; i < count; ++i) {
		distances[i] = float.PositiveInfinity;
		previous[i] = -1; 
	}

	// okay, start with our first guy, who has distance 0;
	DistanceField df = new DistanceField( Width, Height, Costs );
	df.Seed( x, y ); 

	df.SolveTo( maxCost ); 

	for (int i = 0; i < count; ++i) {
		int px, py;
		GetLocation( out px, out py, i ); 
		if (df.GetDistance(px, py) <= maxCost) {
			visitable.Add(i); 
		}
	}

	return visitableTiles;
}
