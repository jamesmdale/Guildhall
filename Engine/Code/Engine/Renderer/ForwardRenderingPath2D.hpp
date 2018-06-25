#pragma once
#include "Engine\Renderer\RenderScene2D.hpp"
#include <vector>

struct DrawCallData2D
{
	~DrawCallData2D()
	{
		m_renderableData = nullptr;		
	}

	Matrix44 m_model;
	RenderableData* m_renderableData;
	int m_sortingLayer;
};

class ForwardRenderingPath2D
{
public:
	ForwardRenderingPath2D();
	~ForwardRenderingPath2D();

	void Render(RenderScene2D* scene);
	void RenderSceneForCamera(Camera* camera, RenderScene2D* scene);
	void SortDrawsBySortOrder(std::vector<DrawCallData2D>& outDrawCalls);
	void SortDrawCallRenderableOrder(std::vector<DrawCallData2D>& outDrawCalls);
};

