#pragma once
#include "Engine\Renderer\RenderScene2D.hpp"
#include <vector>

struct DrawCallData2D
{
	Matrix44 m_model;
	std::vector<Mesh*> m_meshes;
	Material* m_material = nullptr;

	int m_sortingLayer;
};

class ForwardRenderingPath2D
{
public:
	ForwardRenderingPath2D();
	~ForwardRenderingPath2D();

	void Render(RenderScene2D* scene);
	void RenderSceneForCamera(Camera* camera, RenderScene2D* scene);
	void SortDrawsBySortOrder(std::vector<DrawCallData2D> outDrawCalls);
};

