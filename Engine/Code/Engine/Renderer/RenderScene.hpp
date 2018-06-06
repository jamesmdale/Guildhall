#pragma once
#include "Engine\Renderer\Renderable.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <vector>
#include "Engine\Core\Light.hpp"
#include "Engine\Math\Vector3.hpp"


class RenderScene
{
public:
	RenderScene();
	~RenderScene();

	void AddRenderable(Renderable* renderable){m_renderables.push_back(renderable);}
	void AddCamera(Camera* camera){m_cameras.push_back(camera);}
	void AddLight(Light* light){m_lights.push_back(light);}

	void RemoveRenderable(Renderable* renderableToRemove);
	void RemoveCamera(Camera* cameraToRemove);
	void RemoveLight(Light* lightToRemove);

public:
	//max of MAX_LIGHTS
	std::vector<Light*> GetMostContributingLights(Vector3 renderablePosition);

	std::vector<Renderable*> m_renderables;
	std::vector<Camera*> m_cameras;
	std::vector<Light*> m_lights;
};

