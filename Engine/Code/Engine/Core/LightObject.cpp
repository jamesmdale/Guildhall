#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\RendererTypes.hpp"
#include "Engine\Math\Vector4.hpp"


LightObject::LightObject()
{
	m_light = new Light();
	m_light->SetToDefault();
}

LightObject::~LightObject()
{
	if (m_light != nullptr)
	{
		delete(m_light);
		m_light = nullptr;
	}

	m_renderScene->RemoveLightObject(this);	
}

void LightObject::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	return;
}

void LightObject::UpdateLightFromWorldTransform()
{
	m_light->m_lightForward = m_transform->GetWorldForward();
	m_light->m_lightPosition = m_transform->GetWorldPosition();
}

Rgba LightObject::GetLightColor()
{
	return Rgba(m_light->m_colorAndIntensity.x, m_light->m_colorAndIntensity.y, m_light->m_colorAndIntensity.z, 1.f);
}
