#include "Engine\Core\Widget.hpp"
#include "Engine\Renderer\Renderer.hpp"

Widget::Widget()
{

}

Widget::~Widget()
{
	m_renderScene->RemoveRenderable(m_renderable2D);

	if(m_renderable2D != nullptr)
	{
		delete(m_renderable2D);
		m_renderable2D = nullptr;
	}

	delete(m_transform2D);
	m_transform2D = nullptr;

	m_renderScene = nullptr;	
}

Widget::Widget(std::string name)
{
	m_name = name;
	m_renderable2D = new Renderable2D();
	m_transform2D = new Transform2D();
}

Widget::Widget(std::string name, Renderable2D* renderable2D)
{
	m_name = name;
	m_renderable2D = renderable2D;
	m_transform2D = new Transform2D();
}

void Widget::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Widget::PreRender()
{
	if (m_transform2D->IsTransform2DDirty())
	{
		UpdateRenderable2DFromTransform();
	}
}

void Widget::UpdateRenderable2DFromTransform()
{
	m_renderable2D->SetModelMatrix(m_transform2D->GetWorldMatrix());
}


