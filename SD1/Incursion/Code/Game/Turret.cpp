#include "Turret.hpp"
#include "GameCommon.hpp"

void Turret::Update(float timeDelta)
{	
	if(m_health <= 0.f)
	{
		m_isDead = true;
	}
	if(!m_isDead)
	{
		if(!m_isReadyToShoot)
		{
			m_shootCooldown += timeDelta;
			if(m_shootCooldown >= g_turretShootCooldown)
			{
				m_isReadyToShoot = true;
				m_shootCooldown = 0.f;
			}
		}
		
		if(m_doesHaveTargetLineOfSight)
		{
			m_turretRotationDegrees = TurnToward(m_turretRotationDegrees, m_targetDegrees, 30.f * timeDelta);
		}
		else
		{
			m_goalCooldown += timeDelta;
			if(m_goalCooldown >= g_enemyTankGoalCooldown)
			{				
				m_goalCooldown = 0.f;
				m_targetDegrees = GetRandomFloatInRange(0.f, 360.f);	
				m_turretRotationDegrees = TurnToward(m_turretRotationDegrees, m_targetDegrees, 30.f * timeDelta);
				
			}
			m_turretRotationDegrees = TurnToward(m_turretRotationDegrees, m_targetDegrees, 30.f * timeDelta);
			
		}

		m_age += timeDelta;
		m_turretOrientationVector = Vector2(CosDegrees(m_turretRotationDegrees), SinDegrees(m_turretRotationDegrees)).GetNormalized();

		m_innerCollider.center = m_center;
		m_outerCollider.center = m_center;
		m_innerCollider.radius = 0.8f * m_cosmeticRadius;
		m_outerCollider.radius = 1.f * m_cosmeticRadius;
	}
}

void Turret::Render()
{
	if(g_isDebug == true)
	{
		g_theRenderer->DrawDottedDisc2WithColor(m_innerCollider, g_debugInnerColliderColor, g_defaultCircleSides);
		g_theRenderer->DrawDottedDisc2WithColor(m_outerCollider, g_debugOuterColliderColor, g_defaultCircleSides);
	}
	
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_center);	
	g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	Texture* baseTexture = g_theRenderer->CreateOrGetTexture(m_baseTexture);
	g_theRenderer->DrawTexturedAABB(m_textureBounds, *baseTexture, Vector2(0, 1), Vector2(1, 0), m_color);
	g_theRenderer->PopMatrix();	

	g_theRenderer->DrawLineWithColorFeathered(m_center, m_center + (m_turretOrientationVector * m_turretLaserDrawDistance), g_turretLaserColorStart, g_turretLaserColorEnd);
	
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_center);	
	g_theRenderer->Rotatef(m_turretRotationDegrees, 0, 0, 1);	
	g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	Texture* turretTexture = g_theRenderer->CreateOrGetTexture(m_turretTexture);
	g_theRenderer->DrawTexturedAABB(m_textureBounds, *turretTexture, Vector2(0, 1), Vector2(1, 0), m_color);
	g_theRenderer->PopMatrix();
}