#pragma once
#include "Engine\Core\Transform.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include <string>
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\RenderScene.hpp"
#include <vector>

class GameObject
{
public:
	GameObject();
	GameObject(std::string name);
	GameObject(std::string name, Renderable* renderable);

	virtual ~GameObject();	

	virtual void Update(float deltaSeconds);

	void AddRenderable(Renderable* renderable);
	void DeleteRenderables();
	
public:
	std::string m_name = "";
	std::vector<Renderable*> m_renderables;
	Transform* m_transform = nullptr;
	RenderScene* m_renderScene = nullptr;
};


