#include "Engine\Renderer\ForwardRenderingPath2D.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "ForwardRenderingPath.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Time\Clock.hpp"


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
		for (int dataIndex = 0; dataIndex < (int)renderable->m_renderableData.size(); ++dataIndex)
		{		
			DrawCallData2D drawCall;

			drawCall.m_renderableData  = renderable->m_renderableData[dataIndex];
			drawCall.m_model = renderable->GetModelMatrix();
			drawCall.m_sortingLayer = renderable->m_widgetSortLayer;

			drawCalls.push_back(drawCall);
		}	
	}

	SortDrawsBySortOrder(drawCalls);
	SortDrawCallRenderableOrder(drawCalls);

	for (DrawCallData2D drawCall : drawCalls)
	{	
		theRenderer->BindMaterial(drawCall.m_renderableData->m_material);

		int handle = drawCall.m_renderableData->m_material->GetShaderProgramHandle();
		if (handle > -1)
		{
			Vector3 cameraPosition = camera->m_transform->GetWorldPosition();
			theRenderer->SetVector3Uniform(handle, "EYE_POSITION", cameraPosition);
			//theRenderer->SetVector3Uniform(handle, "TIME", Vector3((float)GetMasterClock()->GetRunningTime(), 0.f, 0.f));
		}
		else
			GUARANTEE_OR_DIE(handle > -1, "HANDLE FOR DRAW CALL IS = -1");

		theRenderer->DrawMesh(drawCall.m_renderableData->m_mesh, drawCall.m_model);

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

void ForwardRenderingPath2D::SortDrawCallRenderableOrder(std::vector<DrawCallData2D>& outDrawCalls)
{
	for (int i = 0; i < (int)outDrawCalls.size(); i++)
	{
		for (int j = i + 1; j < (int)outDrawCalls.size(); j++)
		{
			if (outDrawCalls[j].m_sortingLayer < outDrawCalls[i].m_sortingLayer)
			{
				break;
			}

			//make sure the drawcalls are on the same layer first
			if (outDrawCalls[j].m_sortingLayer == outDrawCalls[i].m_sortingLayer)
			{
				if (outDrawCalls[j].m_renderableData->m_meshDrawOrder < outDrawCalls[i].m_renderableData->m_meshDrawOrder)
				{
					std::swap(outDrawCalls[j], outDrawCalls[i]);
				}
			}
		}
	}
}
