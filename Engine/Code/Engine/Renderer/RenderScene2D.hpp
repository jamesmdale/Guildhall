#pragma once
#include "Engine\Renderer\Renderable2D.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <vector>
#include "Engine\Core\Light.hpp"
#include "Engine\Math\Vector2.hpp"


class RenderScene2D
{
public:
	RenderScene2D();
	~RenderScene2D();

	void AddRenderable(Renderable2D* renderable2D){m_renderables.push_back(renderable2D);}
	void AddCamera(Camera* camera){m_cameras.push_back(camera);};

	void RemoveRenderable(Renderable2D* renderableToRemove);
	void RemoveCamera(Camera* cameraToRemove);

public:
	std::vector<Renderable2D*> m_renderables;
	std::vector<Camera*> m_cameras;

};

