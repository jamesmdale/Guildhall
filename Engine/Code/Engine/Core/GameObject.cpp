#include "Engine\Core\GameObject.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"

GameObject::GameObject()
{
	m_transform = new Transform();
}

GameObject::GameObject(std::string name)
{
	m_name = name;
	m_transform = new Transform();
}


GameObject::GameObject(std::string name, Renderable* renderable)
{
	m_name = name;
	m_renderables.push_back(renderable);
	m_transform = new Transform();
}

GameObject::~GameObject()
{	
	DeleteRenderables();

	delete(m_transform);
	m_transform = nullptr;

	m_renderScene = nullptr;
}

void GameObject::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	return;
}

void GameObject::AddRenderable(Renderable * renderable)
{
	m_renderables.push_back(renderable);
}

void GameObject::DeleteRenderables()
{
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->RemoveRenderable(m_renderables[renderableIndex]);

		if (m_renderables[renderableIndex] != nullptr)
		{
			delete(m_renderables[renderableIndex]);
			m_renderables[renderableIndex] = nullptr;
		}
	}
	m_renderables.clear();
}
