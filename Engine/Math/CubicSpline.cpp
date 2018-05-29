#include "Engine/Math/CubicSpline.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicSpline2D::CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray)
{
	AppendPoints(positionsArray, numPoints, velocitiesArray);
}
// Mutators
void CubicSpline2D::AppendPoint( const Vector2& position, const Vector2& velocity)
{
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}
void CubicSpline2D::AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray)
{
	for(int pointIndex = 0; pointIndex < numPoints; pointIndex++)
	{
		m_positions.push_back(*positionsArray);		

		if(velocitiesArray != nullptr)
		{
			m_velocities.push_back(*velocitiesArray);
		}
		else
		{
			m_velocities.push_back(Vector2(0.f,0.f));
		}		

		if(pointIndex + 1 < numPoints)
		{
			positionsArray++;

			if(velocitiesArray != nullptr)
			{
				velocitiesArray++;
			}			
		}		
	}
}
void CubicSpline2D::InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity)
{
	m_positions.insert(m_positions.begin() + insertBeforeIndex, position);
	m_velocities.insert(m_velocities.begin() + insertBeforeIndex, velocity);
}
void CubicSpline2D::RemovePoint( int pointIndex )
{
	m_positions.erase(m_positions.begin() + pointIndex);
	m_velocities.erase(m_velocities.begin() + pointIndex);
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.clear();
	m_velocities.clear();
}
void CubicSpline2D::SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity )
{
	m_positions.at(pointIndex) = newPosition;
	m_velocities.at(pointIndex) = newVelocity;
}
void CubicSpline2D::SetPosition( int pointIndex, const Vector2& newPosition )
{
	m_positions.at(pointIndex) = newPosition;
}
void CubicSpline2D::SetVelocity( int pointIndex, const Vector2& newVelocity )
{
	m_velocities.at(pointIndex) = newVelocity;
}
void CubicSpline2D::SetCardinalVelocities( float tension, const Vector2& startVelocity, const Vector2& endVelocity)
{
	m_velocities[0] = startVelocity;
	m_velocities[m_velocities.size()-1] = endVelocity;

	for(size_t velIndex = 1; velIndex < m_velocities.size() - 1; velIndex++)
	{
		m_velocities[velIndex] = (m_positions[velIndex + 1] - m_positions[velIndex - 1]) * .5f * (1 - tension);
	}
}

Vector2 CubicSpline2D::GetPosition( int pointIndex ) const
{
	return m_positions.at(pointIndex);
}
Vector2	CubicSpline2D::GetVelocity( int pointIndex ) const
{
	return m_velocities.at(pointIndex);
}
int	CubicSpline2D::GetPositions( std::vector<Vector2>& out_positions ) const
{
	out_positions = m_positions;
	return (int)out_positions.size();
}
int	CubicSpline2D::GetVelocities( std::vector<Vector2>& out_velocities ) const
{
	out_velocities = m_velocities;
	return (int)m_velocities.size();
}
Vector2	CubicSpline2D::EvaluateAtCumulativeParametric( float t ) const
{
	float floorValue = (float)((int)std::floor(t));
	float matchingIndex  = floorValue + 1.f;
	float difference = 1 - (matchingIndex - t);

	Vector2 parametricVector = EvaluateCubicHermite(m_positions[(int)floorValue], m_velocities[(int)floorValue], m_positions[(int)matchingIndex], m_velocities[(int)matchingIndex], difference);
	return parametricVector;
}
Vector2	CubicSpline2D::EvaluateAtNormalizedParametric( float t ) const
{
	float parametric = RangeMapFloat(t, 0.f, 1.f, 0.f, (float)m_positions.size());
	Vector2 parametricVector = EvaluateAtCumulativeParametric(parametric);
	/*Vector2 parametricVector = EvaluateCubicHermite(m_positions[(int)floorValue], m_velocities[(int)floorValue], m_positions[(int)matchingIndex], m_velocities[(int)matchingIndex], difference);
	return parametricVector;*/

	return parametricVector;
}