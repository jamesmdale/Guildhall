//
//#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include "entity.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"
#include <math.h>
#include "engine\math\vector2.hpp"
#include "engine\math\disc2.hpp"

Entity::Entity(float rotationInDegrees, float rotationSpeed, float movementVelocity, float cosmeticRadius, float radius, float centerX, float centerY, float age, Disc2 innerCollider, Disc2 outerCollider, Vector2 movementVector, Vector2 shapeVertices[g_MaxVerticesPerShape], Rgba color, char type)
{	
	m_rotationDegrees = rotationInDegrees;	
	m_rotationSpeed = rotationSpeed;
	m_movementSpeed = movementVelocity;
	m_cosmeticRadius = cosmeticRadius;
	m_radius = radius; //NOTE:should always be equal to 1?
	m_centerX = centerX;
	m_centerY = centerY;
	m_age = age;
	m_innerCollider = innerCollider;
	m_outerCollider = outerCollider;
	m_movementVector = movementVector;
	m_shapeVertices[g_MaxVerticesPerShape] = shapeVertices[g_MaxVerticesPerShape];
	m_color = color;
	m_Type = type;
	
}

Entity::Entity(const Entity& copyFrom)
{
	m_rotationDegrees = copyFrom.m_rotationDegrees;	
	m_rotationSpeed = copyFrom.m_rotationSpeed;
	m_movementSpeed = copyFrom.m_movementSpeed;
	m_radius = copyFrom.m_radius;
	m_centerX = copyFrom.m_centerX;
	m_centerY = copyFrom.m_centerY;
	m_age = copyFrom.m_age;
	m_innerCollider = copyFrom.m_innerCollider;
	m_outerCollider = copyFrom.m_outerCollider;
	m_movementVector = copyFrom.m_movementVector;
	m_shapeVertices[g_MaxVerticesPerShape] = copyFrom.m_shapeVertices[g_MaxVerticesPerShape];
	m_color = copyFrom.m_color;
}

void Entity::Initialize(){}

void Entity::Update(float timeDelta)
{	
	m_age += timeDelta;
	m_rotationDegrees = m_rotationDegrees + (m_rotationSpeed * timeDelta);
	m_centerX += (m_movementVector.x * (m_movementSpeed * timeDelta));
	m_centerY += (m_movementVector.y * (m_movementSpeed * timeDelta));
	m_innerCollider.center = Vector2(m_centerX, m_centerY);
	m_outerCollider.center = Vector2(m_centerX, m_centerY);
	m_innerCollider.radius = 0.8f * m_cosmeticRadius;
	m_outerCollider.radius = 1.f * m_cosmeticRadius;
	WrapObjectAroundScreen();

	if(m_isExploding)
	{
		float explosionTime = 0;
		if(m_Type == 'a')
		{
			explosionTime = g_explosionTimeAsteroid;
		}
		if(m_Type == 's')
		{
			explosionTime = g_explosionTimeShip;
		}
		if(m_Type == 'b')
		{
			explosionTime = g_explosionTimeBullet;
		}

		if(m_age >= (m_explodeTime + explosionTime))
		{
			m_isDead = true;
			m_isExploding = false;
		}		
		ExplodeUpdate(timeDelta);
	}
}

void Entity::Render()
{
	if(g_isDebug == true)
	{
		g_theRenderer->DrawDottedDisc2WithColor(m_innerCollider, g_debugInnerColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawDottedDisc2WithColor(m_outerCollider, g_debugOuterColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawLineWithColor(Vector2(m_centerX, m_centerY), CalculateDebugVector(), g_debugMovementVectorColor);
	}
	if(!m_isDead && !m_isExploding)
	{
		DrawShapeWithVertices();
	}
	else if( m_isExploding)
	{
		DrawExplodingShapeWithVertices();
	}
}

Vector2 Entity::CalculateDebugVector()
{
	Vector2 debugMovementVector = Vector2(m_centerX, m_centerY);
	for(int i = 0; i < 60; i++)
	{
		debugMovementVector += m_movementVector;
	}

	return debugMovementVector;
}

void Entity::DrawShapeWithVertices()
{				
	g_theRenderer ->PushMatrix();	
	g_theRenderer ->Translatef(Vector2(m_centerX, m_centerY));	
	g_theRenderer ->Rotatef(m_rotationDegrees, 0, 0, 1);	
	g_theRenderer ->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	g_theRenderer ->BeginLines();
	float numVertices = 0;
	
	if(m_Type == 'a')
	{
		numVertices = g_MaxVerticesPerShape;
	}
	if(m_Type == 's')
	{
		numVertices = g_numShipVertices;
	}
	if(m_Type == 'b')
	{
		numVertices = g_numBulletVertices;
	}
	for(int i = 0; i < numVertices; ++ i)
	{	
		if (i != numVertices - 1) //if we aren't at the end of the array draw to the next element
		{
			g_theRenderer->DrawLineWithColor( Vector2(m_shapeVertices[i].x, m_shapeVertices[i].y), Vector2(m_shapeVertices[i + 1].x, m_shapeVertices[i + 1].y), m_color);			
		}
		else //if we are at the end of the array, draw the last vertex to the first element to complete the shape
		{
			g_theRenderer->DrawLineWithColor( Vector2(m_shapeVertices[i].x, m_shapeVertices[i].y), Vector2(m_shapeVertices[0].x, m_shapeVertices[0].y), m_color);	
			break;
		}			
	}
	g_theRenderer->End();
	g_theRenderer->PopMatrix();	
}

void Entity::DrawExplodingShapeWithVertices()
{				
	g_theRenderer ->PushMatrix();	
	g_theRenderer ->Translatef(Vector2(m_centerX, m_centerY));	
	g_theRenderer ->Rotatef(m_rotationDegrees, 0, 0, 1);	
	g_theRenderer ->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	g_theRenderer ->BeginLines();
	float numVertices = 0;

	
	if(m_Type == 'a')
	{
		numVertices = g_MaxVerticesPerShape;
	}
	if(m_Type == 's')
	{
		numVertices = g_numShipVertices;
	}
	if(m_Type == 'b')
	{
		numVertices = g_numBulletVertices;
	}
	numVertices *= 2;
	for(int i = 0; i < numVertices; i+=2)
	{	
		if (i < numVertices)
		{
			g_theRenderer->DrawLineWithColor( Vector2(m_shapeVerticesExploding[i].x, m_shapeVerticesExploding[i].y), Vector2(m_shapeVerticesExploding[i + 1].x, m_shapeVerticesExploding[i + 1].y), m_color);
		}
	}
	g_theRenderer->End();
	g_theRenderer->PopMatrix();	
}

void Entity::CloneVerticesToExplodeVertices()
{
	float numVertices = 0;
	if(m_Type == 'a')
	{
		numVertices = g_MaxVerticesPerShape;
	}
	if(m_Type == 's')
	{
		numVertices = g_numShipVertices;
	}
	if(m_Type == 'b')
	{
		numVertices = g_numBulletVertices;
	}
	
	for(int i = 0, j= 0; i < numVertices; ++ i, j+=2) //we need to store each vertex for each line. This means we have double the vectors stored in this array
	{
		if(i == numVertices - 1)
		{
			m_shapeVerticesExploding[j] = m_shapeVertices[i];
			m_shapeVerticesExploding[j + 1] = m_shapeVertices[0];							
		}		
		else
		{
			m_shapeVerticesExploding[j] = m_shapeVertices[i];
			m_shapeVerticesExploding[j + 1] = m_shapeVertices[i+1];		
		}
	}
}

void Entity::ExplodeUpdate(float timeDelta)
{
	float numVertices = 0;
	float explosionSpeed = 0;
	if(m_Type == 'a')
	{
		explosionSpeed = g_explosionSpeedAsteroid;
		numVertices = g_MaxVerticesPerShape;
	}
	if(m_Type == 's')
	{
		explosionSpeed = g_explosionSpeedShip;
		numVertices = g_numShipVertices;
	}
	if(m_Type == 'b')
	{
		explosionSpeed = g_explosionSpeedBullet;
		numVertices = g_numBulletVertices;
	}
	numVertices *= 2;
	for(int i = 0; i < numVertices; i+=2) //we need to store each vertex for each line. This means we have double the vectors stored in this array
	{	
		if (i < numVertices) //if we aren't at the end of the array draw to the next element
		{
			Vector2 lineMidpoint = GetMidpoint(m_shapeVerticesExploding[i], m_shapeVerticesExploding[i + 1]);
			Vector2 lineExplosionDirection = lineMidpoint; //gives us the direction vector

			lineExplosionDirection.NormalizeAndGetLength();

			Vector2 vertexTranslation = lineExplosionDirection * (explosionSpeed * timeDelta);

			m_shapeVerticesExploding[i] += vertexTranslation;
			m_shapeVerticesExploding[i + 1] += vertexTranslation;			
		}
	}
}

void Entity::WrapObjectAroundScreen()
{
	float boundsMax = 1000 + m_cosmeticRadius; //factor radius of object in calc
	float boundsMin = 0 - m_cosmeticRadius;
	if(m_centerX >  boundsMax) 
	{
		m_centerX -= (boundsMax + m_cosmeticRadius);
	}
	if(m_centerY > boundsMax)
	{
		m_centerY -= (boundsMax + m_cosmeticRadius);
	}
	if(m_centerX < boundsMin)
	{
		m_centerX += (boundsMax + m_cosmeticRadius);
	}
	if(m_centerY < boundsMin)
	{
		m_centerY += (boundsMax + m_cosmeticRadius);
	}
}