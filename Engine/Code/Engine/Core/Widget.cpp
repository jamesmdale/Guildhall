#include "Engine\Core\Widget.hpp"
#include "Engine\Renderer\Renderer.hpp"

Widget::Widget()
{
	m_transform2D = new Transform2D();
	UpdateSortLayer(-1);
}

Widget::~Widget()
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

	delete(m_transform2D);
	m_transform2D = nullptr;

	m_renderScene = nullptr;	
}

Widget::Widget(std::string name)
{
	m_name = name;
	m_transform2D = new Transform2D();
	UpdateSortLayer(-1);
}

Widget::Widget(std::string name, Renderable2D* renderable2D)
{
	UNUSED(renderable2D);

	m_name = name;
	m_transform2D = new Transform2D();
	UpdateSortLayer(-1);
}

void Widget::Initialize()
{
	//do initialization tasks here
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
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderables[renderableIndex]->SetModelMatrix(m_transform2D->GetWorldMatrix());
	}	
}

void Widget::DeleteRenderables()
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

	if (m_renderables.size() > 0)
		m_renderables.clear();
}

void Widget::UpdateSortLayer(int sortLayer)
{
	m_sortLayer = sortLayer;
	
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderables[renderableIndex]->m_widgetSortLayer = sortLayer;
	}
}

void Widget::AddRenderable(Renderable2D* renderable)
{
	renderable->m_widgetSortLayer = m_sortLayer;
	m_renderables.push_back(renderable);
}

//handle mouse input
void Widget::OnLeftReleased(){}
void Widget::OnRightReleased(){}

void Widget::OnLeftClicked(){}
void Widget::OnRightClicked(){}

void Widget::OnDoubleClickedLeft(){}
void Widget::OnDoubleClickedRight(){}

void Widget::OnLeftDragged(){}
void Widget::OnRightDragged(){}

