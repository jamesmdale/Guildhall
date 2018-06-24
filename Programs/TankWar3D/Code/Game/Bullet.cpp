#include "Game\Bullet.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Game\Spawner.hpp"
#include "Game\Swarmer.hpp"
#include "Engine\Audio\AudioSystem.hpp"

//defaults to set in constructor
float defaultTimeToLive = 5.f;
float defaultMovementSpeed = 30.f;
float defaultLightIntensity = 20.f;

Bullet::Bullet(const Vector3& startingPosition, const Vector3& startingRotation)
{
	m_transform->SetLocalPosition(startingPosition);
	m_transform->SetLocalRotation(startingRotation);

	m_timeToLive = 5.f; //hard set for now.

	Rgba lightColor = Rgba::RED;

	m_light = new Light();
	m_light->m_attenuation = Vector3::DEFAULT_LIGHT_ATTENUATION;

	Rgba defaultLightColor = Rgba::RED;
	m_light->m_colorAndIntensity = Rgba::ConvertToVector4(defaultLightColor);
	m_light->m_colorAndIntensity.w = defaultLightIntensity;

	UpdateLightDataFromWorldTransform();
}

Bullet::~Bullet()
{
	m_renderScene->RemoveLight(m_light);

	if (m_light != nullptr)
	{
		delete(m_light);
		m_light = nullptr;
	}

}

void Bullet::Update(float deltaSeconds)
{
	m_timeToLive -= deltaSeconds;

	if (m_timeToLive <= 0.0f)
	{
		m_isDead = true;
	}

	//update movement
	Vector3 positionToAdd = m_transform->GetWorldForward() * deltaSeconds * defaultMovementSpeed;
	m_transform->TranslatePosition(positionToAdd);
	
	Vector3 currentPosition = m_transform->GetWorldPosition();

	float terrainHeightAtPosition = m_gameState->m_terrain->GetHeightAtPositionXZ(Vector2(currentPosition.x, currentPosition.z));
	
	if (terrainHeightAtPosition >= currentPosition.y)
	{
		MarkAsDead();
		AudioSystem::GetInstance()->PlaySoundFromGroup("impact");
		return;
	}

	for (int spawnerIndex = 0; spawnerIndex < (int)m_gameState->m_spawners.size(); ++spawnerIndex)
	{
		Vector3 spawnerPosition = m_gameState->m_spawners[spawnerIndex]->m_transform->GetWorldPosition();

		AABB3 spawnerBounds = AABB3(spawnerPosition, g_spawnerDimensions.x * 0.5f, g_spawnerDimensions.y * 0.5f, g_spawnerDimensions.z * 0.5f);

		if (spawnerBounds.IsPointInside(currentPosition))
		{
			MarkAsDead();
			AudioSystem::GetInstance()->PlaySoundFromGroup("impact");
			m_gameState->m_spawners[spawnerIndex]->m_health--;
			return;
		}
	}

	for (int swarmerIndex = 0; swarmerIndex < (int)m_gameState->m_swarmers.size(); ++swarmerIndex)
	{
		Vector3 swarmerPosition = m_gameState->m_swarmers[swarmerIndex]->m_transform->GetWorldPosition();

		if (GetDistanceSquared(currentPosition, swarmerPosition) < g_swarmerRadius * g_swarmerRadius)
		{
			MarkAsDead();
			AudioSystem::GetInstance()->PlaySoundFromGroup("impact");
			m_gameState->m_swarmers[swarmerIndex]->m_health--;
			return;
		}
	}

	UpdateLightDataFromWorldTransform();
}

void Bullet::UpdateLightDataFromWorldTransform()
{
	m_light->m_lightForward = m_transform->GetWorldForward();
	m_light->m_lightPosition = m_transform->GetWorldPosition();
}

Rgba Bullet::GetLightColor()
{
	return Rgba(m_light->m_colorAndIntensity.x, m_light->m_colorAndIntensity.y, m_light->m_colorAndIntensity.z, 1.f);
}
