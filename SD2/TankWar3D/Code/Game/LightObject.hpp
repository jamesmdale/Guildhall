#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Light.hpp"
#include "Engine\Core\Rgba.hpp"

enum LightType
{
	LIGHT_TYPE_POINT_LIGHT,
	LIGHT_TYPE_DIRECTIONAL_LIGHT,
	LIGHT_TYPE_SPOT_LIGHT,
	NUM_LIGHT_TYPES
};

class LightObject : public GameObject
{
public:
	LightObject();

	LightObject(const std::string& name, const LightType& lightType) : GameObject(name)
	{
		m_light = new Light();
		m_lightType = lightType;	
	}

	LightObject(const std::string& name, const LightType& lightType, const Rgba& color, float intensity, const Vector3& attenuation, float directionFactor, float innerAngle, float outerAngle) : GameObject(name)
	{
		m_light = new Light();

		m_lightType = lightType;
		m_light->m_colorAndIntensity = Rgba::ConvertToVector4(color);
		m_light->m_colorAndIntensity.w = intensity;
		m_light->m_attenuation = attenuation;
		m_light->m_lightDirectionFactor = directionFactor;
		m_light->m_lightInnerAngle = innerAngle;
		m_light->m_lightOuterAngle = outerAngle;
	}

	~LightObject();

	virtual void Update(float deltaTime) override;
	virtual void PreRender() override;
	void UpdateLightDataFromWorldTransform();
	Rgba GetLightColor();

public:

	TODO("Might want to remove this and make lights work regardless of type. (Fill in spot constants and direction in all scenarious)");
	LightType m_lightType = LIGHT_TYPE_POINT_LIGHT;	
	Light* m_light = nullptr;
};

