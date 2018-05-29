#include "AirStrike.hpp"
#include "GameCommon.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Core\Rgba.hpp"

AirStrike::AirStrike(Vector2 position, float radius, float duration)
{
	m_position = position;	
	m_maxRadius = radius;
	m_duration = duration;
	m_isExploding = false;

	m_currentRadius = 0.0f;
	m_elapsedSeconds = 0.0f;
	m_explosionStartTime = 0.0f;
	m_readyForDelete = false;
	m_cosmeticRadius = .5f;
	
}

void AirStrike:: Update(float deltaTime)
{
	if(!m_isExploding)
	{
		m_elapsedSeconds += deltaTime;

		float currentRadiusInSecondsFraction = m_elapsedSeconds/(float)m_duration;

		m_currentRadius = currentRadiusInSecondsFraction * m_maxRadius;
	}
	else
	{
		m_explosionStartTime += deltaTime;
		if(m_explosionStartTime >= g_airStrikeExplosionTime)
		{
			m_readyForDelete = true;
		}
	}
	
	if(m_currentRadius >= m_maxRadius)
	{
		m_isExploding = true;
	}
}

void AirStrike::Render()
{
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Scalef(m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius);
	Texture* texture = g_theRenderer->CreateOrGetTexture("Data/Images/AirStrike.png");
	g_theRenderer->DrawTexturedAABB(AABB2(Vector2(0.f, 0.f), m_currentRadius, m_currentRadius), *texture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, .5f));
	g_theRenderer->PopMatrix();	
}