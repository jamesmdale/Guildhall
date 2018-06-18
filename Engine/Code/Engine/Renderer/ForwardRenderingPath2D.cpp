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
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	std::vector<DrawCallData2D> drawCalls;

	for (Renderable2D* renderable : scene->m_renderables)
	{
		DrawCallData2D drawCall;

		for (int dataIndex = 0; dataIndex < (int)renderable->m_renderableData.size(); ++dataIndex)
		{		
			drawCall.m_renderableData.push_back(renderable->m_renderableData[dataIndex]);
			drawCall.m_model = renderable->GetModelMatrix();
			drawCall.m_sortingLayer = renderable->m_widgetSortLayer;

			drawCalls.push_back(drawCall);
		}	
	}

	SortDrawsBySortOrder(drawCalls);
	//SortDrawsByMeshSortOrder(drawCalls);

	for (DrawCallData2D drawCall : drawCalls)
	{	
		for (int dataIndex = 0; dataIndex < (int)drawCall.m_renderableData.size(); ++dataIndex)
		{
			theRenderer->BindMaterial(drawCall.m_renderableData[dataIndex]->m_material);

			int handle = drawCall.m_renderableData[dataIndex]->m_material->GetShaderProgramHandle();
			if (handle > -1)
			{
				Vector3 cameraPosition = camera->m_transform->GetWorldPosition();
				theRenderer->SetVector3Uniform(handle, "EYE_POSITION", cameraPosition);
			}

			theRenderer->DrawMesh(drawCall.m_renderableData[dataIndex]->m_mesh, drawCall.m_model);			
		}
	}

	theRenderer = nullptr;
}

void ForwardRenderingPath2D::SortDrawsBySortOrder(std::vector<DrawCallData2D>& outDrawCalls)
{
	for(int i = 0; i < (int)outDrawCalls.size(); i++)
	{
		for(int j = i + 1; j < (int)outDrawCalls.size(); j++)
		{
			if(outDrawCalls[j].m_sortingLayer < outDrawCalls[i].m_sortingLayer)
			{				
				std::swap(outDrawCalls[j], outDrawCalls[i]);
			}
		}
	}
}
