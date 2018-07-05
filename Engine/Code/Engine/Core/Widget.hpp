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

	void UpdateSortLayer(int sortLayer);
	int GetSortLayer(){return m_sortLayer;}

	void AddRenderable(Renderable2D* renderable);

	//input udpates
	virtual void OnLeftReleased();
	virtual void OnRightReleased();

	virtual void OnLeftClicked();
	virtual void OnRightClicked();

	virtual void OnDoubleClickedLeft();
	virtual void OnDoubleClickedRight();

	virtual void OnLeftDragged();
	virtual void OnRightDragged();

public:
	std::string m_name = "";
	std::vector<Renderable2D*> m_renderables;
	Transform2D* m_transform2D = nullptr;

	Vector2 m_dimensionsInPixels;

	bool m_isInputPriority = false;
	bool m_isPositionLocked = false;
	Vector2 m_lockPosition;

	RenderScene2D* m_renderScene = nullptr;

private:
	int m_sortLayer = 0;
};

