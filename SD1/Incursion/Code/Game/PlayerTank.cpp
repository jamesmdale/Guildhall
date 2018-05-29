#include "PlayerTank.hpp"
#include "GameCommon.hpp"

void PlayerTank::Update(float timeDelta)
{		
	m_age += timeDelta;
	if(m_health <= 0)
	{
		m_health = 10.f;
		m_deathTime = m_age;
		m_isDead = true;
	}
	if(!m_isDead)
	{		
		if(!m_isReadyToShoot)
		{
			m_shootCooldown += timeDelta;
			if(m_shootCooldown >= g_playerShootCooldown)
			{
				m_isReadyToShoot = true;
				m_shootCooldown = 0.f;
			}
		}	
		m_rotationDegrees = m_rotationDegrees + (m_rotationSpeed * timeDelta);
		m_orientationVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
		m_center += m_movementVector * timeDelta;
		m_turretOrientationVector = Vector2(CosDegrees(m_turretRotationDegrees), SinDegrees(m_turretRotationDegrees)).GetNormalized();
		
		m_innerCollider.center = m_center;
		m_outerCollider.center = m_center;
		m_innerCollider.radius = 0.8f * m_cosmeticRadius;
		m_outerCollider.radius = 1.f * m_cosmeticRadius;
	}
}

void PlayerTank::Render()
{
	if(g_isDebug == true)
	{
		g_theRenderer->DrawDottedDisc2WithColor(m_innerCollider, g_debugInnerColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawDottedDisc2WithColor(m_outerCollider, g_debugOuterColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawLineWithColor(m_center, CalculateDebugVector(), g_debugMovementVectorColor);
	}
	if(!m_isDead)
	{
		g_theRenderer->PushMatrix();	
		g_theRenderer->Translatef(m_center);	
		g_theRenderer->Rotatef(m_rotationDegrees, 0, 0, 1);	
		g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
		g_theRenderer->DrawTexturedAABB(m_tankBounds, *m_baseTexture, Vector2(0, 1), Vector2(1, 0), m_color);
		g_theRenderer->PopMatrix();	

		g_theRenderer->PushMatrix();	
		g_theRenderer->Translatef(m_center);	
		g_theRenderer->Rotatef(m_turretRotationDegrees, 0, 0, 1);	
		g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
		g_theRenderer->DrawTexturedAABB(m_tankBounds, *m_turretTexture, Vector2(0, 1), Vector2(1, 0), m_color);
		g_theRenderer->PopMatrix();
	}
}