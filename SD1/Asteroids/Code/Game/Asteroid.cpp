#include "asteroid.hpp"
#include "Engine\Math\MathUtils.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>


Asteroid::Asteroid(float rotationInDegrees, float rotationSpeed, float movementVelocity, float radius, float centerX, float centerY, Vector2 movementVector)
{
	m_rotationDegrees = rotationInDegrees;	
	m_rotationSpeed = rotationSpeed;
	m_movementSpeed = movementVelocity;
	m_radius = radius;
	m_centerX = centerX;
	m_centerY = centerY;
	m_movementVector = movementVector;
}

Asteroid::Asteroid( const Asteroid& copyFrom )
{
	m_rotationDegrees = copyFrom.m_rotationDegrees;
	m_rotationSpeed = copyFrom.m_rotationSpeed;
	m_movementSpeed = copyFrom.m_movementSpeed;
	m_radius = copyFrom.m_radius;
	m_centerX = copyFrom.m_centerX;
	m_centerY = copyFrom.m_centerY;
	m_movementVector = copyFrom.m_movementVector;
	m_Type = copyFrom.m_Type;
}
void Asteroid::Initialize(){}

void Asteroid::GenerateVertices()
{	 
	float degrees = 360.f/g_MaxVerticesPerShape;

	for(int i = 0; i < g_MaxVerticesPerShape; i++)
	{
		float radians = ConvertDegreesToRadians((float)i * degrees);
		float randomizedRadius = GetRandomFloatInRange(0.8f, 1.f);
		float startX = CalculateXPoint(0, randomizedRadius, radians);
		float startY = CalculateYPoint(0, randomizedRadius, radians);
		
		m_shapeVertices[i] = Vector2(startX, startY);
	}
}



