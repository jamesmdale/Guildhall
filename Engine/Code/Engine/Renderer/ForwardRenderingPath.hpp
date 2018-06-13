#pragma once
#include "Engine\Renderer\RenderScene.hpp"
#include <vector>

/************************************************************************/
/* Draw Call Data                                                       */
/************************************************************************/
struct DrawCallData
{
	Matrix44 m_model;
	std::vector<Mesh*> m_meshes;
	Material* m_material = nullptr;

	std::vector<Light*> m_contributingLights;

	int m_sortingLayer;
	RenderQueueType m_renderType;
};

class ForwardRenderingPath
{
public:
	ForwardRenderingPath();
	~ForwardRenderingPath();

	void Render(RenderScene* scene);
	void RenderSceneForCamera(Camera* camera, RenderScene* scene);
	void SortDrawsBySortOrder(std::vector<DrawCallData> outDrawCalls);
	void SortDrawsByRenderQueue(std::vector<DrawCallData> outDrawCalls);
	void SortDrawsByCameraDistance(std::vector<DrawCallData> outDrawCalls, const Vector3& cameraPosition);
};


