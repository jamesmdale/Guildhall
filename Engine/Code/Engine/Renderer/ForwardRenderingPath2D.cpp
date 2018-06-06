#include "Engine\Renderer\ForwardRenderingPath2D.hpp"
#include "Engine\Renderer\Renderer.hpp"



ForwardRenderingPath2D::ForwardRenderingPath2D()
{
}


ForwardRenderingPath2D::~ForwardRenderingPath2D()
{
}

void ForwardRenderingPath2D::Render(RenderScene2D * scene)
{
	for(Camera* camera : scene->m_cameras)
	{
		RenderSceneForCamera(camera, scene);
	}

	scene = nullptr;
}

void ForwardRenderingPath2D::RenderSceneForCamera(Camera* camera, RenderScene2D* scene)
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(camera);

	std::vector<DrawCallData2D> drawCalls;

	for (Renderable2D* renderable : scene->m_renderables)
	{
		DrawCallData2D drawCall;

		drawCall.m_material = renderable->GetMaterial();
		drawCall.m_mesh = renderable->GetMesh();
		drawCall.m_model = renderable->GetModelMatrix();
		drawCall.m_sortingLayer = renderable->GetRender2DSortLayer();

		drawCalls.push_back(drawCall);
	}

	SortDrawsBySortOrder(drawCalls);

	for (DrawCallData2D drawCall : drawCalls)
	{	
		if(drawCall.m_mesh != nullptr)
		{
			theRenderer->BindMaterial(drawCall.m_material);

			int handle = drawCall.m_material->GetShaderProgramHandle();
			if(handle > -1)
			{
				Vector3 cameraPosition = camera->m_transform->GetWorldPosition();
				theRenderer->SetVector3Uniform(handle, "EYE_POSITION", cameraPosition);
			}	

			theRenderer->DrawMesh(drawCall.m_mesh, drawCall.m_model);
		}		
	}

	theRenderer = nullptr;
}

void ForwardRenderingPath2D::SortDrawsBySortOrder(std::vector<DrawCallData2D> outDrawCalls)
{
	for(int i = 0; i < (int)outDrawCalls.size(); i++)
	{
		for(int j = i + 1; j < (int)outDrawCalls.size(); j++)
		{
			if(outDrawCalls[j].m_sortingLayer > outDrawCalls[i].m_sortingLayer)
			{				
				std::swap(outDrawCalls[j], outDrawCalls[i]);
			}
		}
	}
}
