#include "Game\LightObject.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RendererTypes.hpp"
#include "Engine\Math\Vector4.hpp"


LightObject::LightObject()
{
	
}

LightObject::~LightObject()
{
	m_renderScene->RemoveLight(m_light);

	if(m_light != nullptr)
	{
		delete(m_light);
		m_light = nullptr;
	}	
}

void LightObject::Update(float deltaTime)
{
	UNUSED(deltaTime);
	return;
}

void LightObject::PreRender()
{
	//update renderable model matrix.
	if(m_transform->IsTransformDirty())
	{
		UpdateRenderableFromTransform();
		UpdateLightDataFromWorldTransform();
	}	
}

void LightObject::UpdateLightDataFromWorldTransform()
{
	m_light->m_lightForward = m_transform->GetWorldForward();
	m_light->m_lightPosition = m_transform->GetWorldPosition();
}

Rgba LightObject::GetLightColor()
{
	return Rgba(m_light->m_colorAndIntensity.x, m_light->m_colorAndIntensity.y, m_light->m_colorAndIntensity.z, 1.f);
}
