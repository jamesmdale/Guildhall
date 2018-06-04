#include "Game\Bullet.hpp"
#include "Engine\Core\Rgba.hpp"

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

	if(m_light != nullptr)
	{
		delete(m_light);
		m_light = nullptr;
	}	

}

void Bullet::Update(float timeDelta)
{
	m_timeToLive -= timeDelta;

	if(m_timeToLive <= 0.0f)
	{
		m_isDead = true;
	}

	//update movement
	Vector3 positionToAdd = m_transform->GetWorldForward() * timeDelta * defaultMovementSpeed;
	m_transform->TranslatePosition(positionToAdd);
}

void Bullet::PreRender()
{
	//update renderable model matrix.
	if(m_transform->IsTransformDirty())
	{
		UpdateRenderableFromTransform();
		UpdateLightDataFromWorldTransform();
	}	
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
