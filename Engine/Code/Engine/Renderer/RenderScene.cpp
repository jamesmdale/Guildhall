#include "Engine\Renderer\RenderScene.hpp"



RenderScene::RenderScene()
{
}


RenderScene::~RenderScene()
{
}

void RenderScene::RemoveRenderable(Renderable* renderableToRemove)
{
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); renderableIndex++)
	{
		if(m_renderables[renderableIndex] == renderableToRemove)
		{
			m_renderables.erase(m_renderables.begin() + renderableIndex);
			renderableIndex--;
		}
	}
}

void RenderScene::RemoveCamera(Camera* cameraToRemove)
{
	for (int cameraIndex = 0; cameraIndex < (int)m_cameras.size(); cameraIndex++)
	{
		if(m_cameras[cameraIndex] == cameraToRemove)
		{
			m_cameras.erase(m_cameras.begin() + cameraIndex);
			cameraIndex--;
		}
	}
}

void RenderScene::RemoveLight(Light* lightToRemove)
{
	for (int lightIndex = 0; lightIndex < (int)m_lights.size(); lightIndex++)
	{
		if(m_lights[lightIndex] == lightToRemove)
		{
			m_lights.erase(m_lights.begin() + lightIndex);
			lightIndex--;
		}
	}
}

std::vector<Light*> RenderScene::GetMostContributingLights(Vector3 renderablePosition)
{
	std::vector<float> lightAttenuations;
	std::vector<Light*> contributingLights;	

	//get number of possible contributing lights maxed to MAX_LIGHTS
	int numContributingLights = ClampInt((int)m_lights.size(), 0, MAX_LIGHTS);

	/*if((int)m_lights.size() > MAX_LIGHTS)
	{*/
		//calculate attenuation based on renderable position foreach light
		for (Light* light : m_lights)
		{
			Vector3 lightDirection = light->m_lightPosition - renderablePosition;
			float distanceToLight = lightDirection.GetLength();

			Vector3 lightForward = light->m_lightForward.GetNormalized();

			//float dotAngle = DotProduct(lightForward, -1 * lightDirection);
			float lightIntensity = light->m_colorAndIntensity.w;

			//float angleAttenuation = GLSLSmoothStep(light->m_lightOuterAngle, light->m_lightInnerAngle, dotAngle);
			//angleAttenuation = ClampFloatZeroToOne(angleAttenuation);
		
			//lightIntensity = lightIntensity * angleAttenuation;
			lightDirection = Interpolate(lightDirection, -1.f * lightForward, light->m_lightDirectionFactor);

			float attenuation = lightIntensity / (light->m_attenuation.x + (light->m_attenuation.y * distanceToLight) + (light->m_attenuation.z * distanceToLight * distanceToLight));
		
			lightAttenuations.push_back(attenuation);
		}

		//sort lights based on attenuation
		//lightAttenuation array must be maintained identically to m_lights array
		for(int i = 0; i < (int)m_lights.size() - 1; i++)
		{
			for(int j = 0; j < (int)m_lights.size() - i - 1; j++)
			{
				if(lightAttenuations[j] < lightAttenuations[j + 1])
				{				
					std::swap(lightAttenuations[j], lightAttenuations[j + 1]);
					std::swap(m_lights[j], m_lights[j + 1]);
				}
			}
		}
	//}

	for(int lightIndex = 0; lightIndex < numContributingLights; lightIndex++)
	{
		contributingLights.push_back(m_lights[lightIndex]);
	}

	return contributingLights;
}
