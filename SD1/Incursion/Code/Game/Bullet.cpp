#include "Bullet.hpp"
#include "GameCommon.hpp"

void Bullet::Update(float timeDelta)
{	
	if(m_health <= 0.f)
	{
		m_isDead = true;
	}
	m_age += timeDelta;
	m_rotationDegrees = m_rotationDegrees + (m_rotationSpeed * timeDelta);
	m_center += (m_movementVector * (m_movementSpeed * timeDelta));
	m_innerCollider.center = m_center;
	m_outerCollider.center = m_center;
	m_innerCollider.radius = 0.8f * m_cosmeticRadius;
	m_outerCollider.radius = 1.f * m_cosmeticRadius;
}

void Bullet::Render()
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