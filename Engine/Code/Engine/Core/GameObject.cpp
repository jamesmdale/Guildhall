#include "Engine\Core\GameObject.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"

GameObject::GameObject()
{
	m_renderable = new Renderable();
	m_transform = new Transform();
}

GameObject::GameObject(std::string name)
{
	m_name = name;
	m_renderable = new Renderable();
	m_transform = new Transform();
}


GameObject::GameObject(std::string name, Renderable* renderable)
{
	m_name = name;
	m_renderable = renderable;
	m_transform = new Transform();
}

GameObject::~GameObject()
{	
	m_renderScene->RemoveRenderable(m_renderable);

	if(m_renderable != nullptr)
	{
		delete(m_renderable);
		m_renderable = nullptr;
	}

	delete(m_transform);
	m_transform = nullptr;

	m_renderScene = nullptr;		
}

void GameObject::Update(float deltaTime)
{
	UNUSED(deltaTime);
	return;
}

void GameObject::PreRender()
{
	//update renderable model matrix.
	if(m_transform->IsTransformDirty())
	{
		UpdateRenderableFromTransform();
	}	
}

void GameObject::UpdateRenderableFromTransform()
{
	m_renderable->SetModelMatrix(m_transform->GetWorldMatrix());
}
