#include "Game\PointOfInterest.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Game\GameCommon.hpp"

PointOfInterest::PointOfInterest(const Vector2 & position, float radius)
{
	m_position = position;

	m_boundsDisc.center = position;
	m_boundsDisc.radius = radius;
}

PointOfInterest::~PointOfInterest()
{
}

void PointOfInterest::Update()
{
	TODO("Add some update functionality later...rotation maybe.")
}

void PointOfInterest::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	float length = m_boundsDisc.radius * 0.25f;
	AABB2 textureBounds = AABB2(m_position, length, length);
	theRenderer->DrawTexturedAABB( textureBounds, *theRenderer->m_defaultTexture, Vector2::ZERO, Vector2::ONE, Rgba::RED);

	theRenderer = nullptr;	
}
