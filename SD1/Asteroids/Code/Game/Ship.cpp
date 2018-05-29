#include "entity.hpp"
#include "engine\math\vector2.hpp"
#include "engine\math\disc2.hpp"
#include "game\GameCommon.hpp"
#include "ship.hpp"


void Ship::Update(float timeDelta)
{		
	if(!m_isDead)
	{
		m_age += timeDelta;
		m_rotationDegrees = m_rotationDegrees + (m_rotationSpeed * timeDelta);
		m_orientationVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
	
		if(m_isAccelerating)
		{		
			m_orientationVector *= 5.f;	
			m_movementVector = m_movementVector + m_orientationVector;
			
			float boosterRandomLength = GetRandomFloatZeroToOne();
			m_shapeVertices[3] = (Vector2(boosterRandomLength, 0.f) * -1);
		}
		else
		{
			m_shapeVertices[3] = Vector2(-0.2f, 0.f);
		}
		m_centerX += (m_movementVector.x * timeDelta);
		m_centerY += (m_movementVector.y * timeDelta);
		m_innerCollider.center = Vector2(m_centerX, m_centerY);
		m_outerCollider.center = Vector2(m_centerX, m_centerY);
		m_innerCollider.radius = 0.8f * m_cosmeticRadius;
		m_outerCollider.radius = 1.f * m_cosmeticRadius;
		WrapObjectAroundScreen();
	}
	if(m_isExploding)
	{
		if(m_age >= (m_explodeTime + g_explosionTimeShip))
		{
			m_isDead = true;
			m_isExploding = false;
		}
		
		ExplodeUpdate(timeDelta);
	}
}

void Ship::Initialize(){}

void Ship::ResetShipToOrigin()
{	
	m_centerX = 500.f;
	m_centerY = 500.f;
	m_rotationDegrees = 90.f;
	m_movementSpeed = 0.0f;	
	m_rotationSpeed = 0.0f;
	m_movementVector = Vector2(0,1);
	m_isExploding = false;
	m_explodeTime = 0.f;
	CloneVerticesToExplodeVertices();
	m_age = 0.f;
}

Vector2 Ship::GetShipNose()
{
	return (Vector2(m_centerX, m_centerY) + (m_orientationVector.GetNormalized() * m_cosmeticRadius));
}