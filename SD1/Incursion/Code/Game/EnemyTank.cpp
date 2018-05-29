#include "EnemyTank.hpp"
#include "GameCommon.hpp"

void EnemyTank::Update(float timeDelta)
{		
	if(!m_isDead)
	{
		if(m_health <= 0.f)
		{
			m_isDead = true;
		}
		if(!m_isReadyToShoot)
		{
			m_shootCooldown += timeDelta;
			if(m_shootCooldown >= g_enemyTankShootCooldown)
			{
				m_isReadyToShoot = true;
				m_shootCooldown = 0.f;
			}
		}
		if(m_doesHaveTargetLineOfSight)
		{
			m_rotationDegrees = TurnToward(m_rotationDegrees, m_targetDegrees, 90.f * timeDelta);
			if(m_isChargingPlayer)
			{
				m_movementVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
				m_center += m_movementVector * (timeDelta * m_movementSpeed);	
			}
		}

		if(!m_doesHaveTargetLineOfSight)
		{
			if(m_targetPosition != Vector2(-1, -1)) //if a tank doesn't have a target position, wander.
			{
				m_rotationDegrees = TurnToward(m_rotationDegrees, m_targetDegrees, 90.f * timeDelta);
				if(m_isChargingPlayer)
				{
					m_movementVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
					m_center += m_movementVector * (timeDelta * m_movementSpeed);	
				}
				if(GetDistance(m_targetPosition, m_center) < 2.0f) //if the tank is at the target position but still doesn't have LoS, begin wandering.
				{
					m_targetPosition = Vector2(-1, -1); //we lost the player so remove the target position					
				}
			}
			else
			{
				m_goalCooldown += timeDelta;
				if(m_goalCooldown >= g_enemyTankGoalCooldown)
				{				
					m_goalCooldown = 0.f;
					m_targetDegrees = GetRandomFloatInRange(0.f, 360.f);
					m_movementVector.MakeDirectionAtDegrees(m_rotationDegrees);	

					m_rotationDegrees = TurnToward(m_rotationDegrees, m_targetDegrees, 90.f * timeDelta);

					m_movementVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
					m_center += m_movementVector * (timeDelta * m_movementSpeed);
				}
				m_rotationDegrees = TurnToward(m_rotationDegrees, m_targetDegrees, 90.f * timeDelta);
				m_movementVector = Vector2(CosDegrees(m_rotationDegrees), SinDegrees(m_rotationDegrees)).GetNormalized();
				m_center += m_movementVector * (timeDelta * m_movementSpeed);
			}
		}		

		m_age += timeDelta;			

		m_innerCollider.center = m_center;
		m_outerCollider.center = m_center;
		m_innerCollider.radius = 0.8f * m_cosmeticRadius;
		m_outerCollider.radius = 1.f * m_cosmeticRadius;
	}
	
}

void EnemyTank::Render()
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
	Texture* texture = g_theRenderer->CreateOrGetTexture(m_texture);
	g_theRenderer->DrawTexturedAABB(m_textureBounds, *texture, Vector2(0, 1), Vector2(1, 0), m_color);
	g_theRenderer->PopMatrix();	
	
}