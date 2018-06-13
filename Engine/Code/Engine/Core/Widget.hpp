#pragma once
#include <string>
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Renderer\Renderable2D.hpp"
#include <vector>

class Widget
{
public:
	Widget();
	virtual ~Widget();

	Widget(std::string name);
	Widget(std::string name, Renderable2D* renderable2D);
	
	virtual void Initialize();
	virtual void Update(float deltaSeconds);
	virtual void PreRender();

	void UpdateRenderable2DFromTransform();
	void DeleteRenderables();

public:
	std::string m_name = "";
	std::vector<Renderable2D*> m_renderables;
	Transform2D* m_transform2D = nullptr;

	RenderScene2D* m_renderScene = nullptr;
};

