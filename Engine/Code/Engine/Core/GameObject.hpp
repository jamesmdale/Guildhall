#pragma once
#include "Engine\Core\Transform.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include <string>
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\RenderScene.hpp"

class GameObject
{
public:
	GameObject();
	GameObject(std::string name);
	GameObject(std::string name, Renderable* renderable);

	virtual ~GameObject();	

	virtual void Update(float deltaSeconds);
	virtual void PreRender();	

	void UpdateRenderableFromTransform();
	
public:
	std::string m_name = "";
	Renderable* m_renderable = nullptr;
	Transform* m_transform = nullptr;

	RenderScene* m_renderScene = nullptr;
};


