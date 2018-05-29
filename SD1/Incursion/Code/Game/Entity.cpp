//
//#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include "entity.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"
#include <math.h>
#include "engine\math\vector2.hpp"
#include "engine\math\disc2.hpp"

Entity::Entity(float rotationInDegrees, float rotationSpeed, float movementVelocity, float cosmeticRadius, float radius, Vector2 center, float age, float health, Disc2 innerCollider, Disc2 outerCollider, Vector2 movementVector, Rgba color, char type)
{	
	m_rotationDegrees = rotationInDegrees;	
	m_rotationSpeed = rotationSpeed;
	m_movementSpeed = movementVelocity;
	m_cosmeticRadius = cosmeticRadius;
	m_radius = radius; //NOTE:should always be equal to 1?
	m_center = center;
	m_age = age;
	m_health = health;
	m_innerCollider = innerCollider;
	m_outerCollider = outerCollider;
	m_movementVector = movementVector;
	m_color = color;
	m_Type = type;
}

Entity::Entity(const Entity& copyFrom)
{
	m_rotationDegrees = copyFrom.m_rotationDegrees;	
	m_rotationSpeed = copyFrom.m_rotationSpeed;
	m_movementSpeed = copyFrom.m_movementSpeed;
	m_radius = copyFrom.m_radius;
	m_center = copyFrom.m_center;
	m_age = copyFrom.m_age;
	m_health = copyFrom.m_health;
	m_innerCollider = copyFrom.m_innerCollider;
	m_outerCollider = copyFrom.m_outerCollider;
	m_movementVector = copyFrom.m_movementVector;	
	m_color = copyFrom.m_color;
}

void Entity::Initialize(){}

void Entity::Update(float timeDelta)
{	
	m_age += timeDelta;
	m_rotationDegrees = m_rotationDegrees + (m_rotationSpeed * timeDelta);
	m_center += (m_movementVector * (m_movementSpeed * timeDelta));
	m_innerCollider.center = m_center;
	m_outerCollider.center = m_center;
	m_innerCollider.radius = 0.8f * m_cosmeticRadius;
	m_outerCollider.radius = 1.f * m_cosmeticRadius;
}

void Entity::Render()
{
	if(g_isDebug == true)
	{
		g_theRenderer->DrawDottedDisc2WithColor(m_innerCollider, g_debugInnerColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawDottedDisc2WithColor(m_outerCollider, g_debugOuterColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawLineWithColor(m_center, CalculateDebugVector(), g_debugMovementVectorColor);
	}
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_center);	
	g_theRenderer->Rotatef(m_rotationDegrees, 0, 0, 1);	
	g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	Texture* texture = g_theRenderer->CreateOrGetTexture(m_texture );
	g_theRenderer->DrawTexturedAABB(m_textureBounds, *texture, Vector2(0, 1), Vector2(1, 0), m_color);
	g_theRenderer->PopMatrix();	
}

Vector2 Entity::CalculateDebugVector()
{
	Vector2 debugMovementVector = m_center;
	for(int i = 0; i < 60; i++)
	{
		debugMovementVector += m_movementVector;
	}

	return debugMovementVector;
}
