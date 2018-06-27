#include "Engine\Renderer\ForwardRenderingPath.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\LightObject.hpp"


ForwardRenderingPath::ForwardRenderingPath()
{
}


ForwardRenderingPath::~ForwardRenderingPath()
{
}

void ForwardRenderingPath::Render(RenderScene* scene)
{
	for (int lightIndex = 0; lightIndex < (int)scene->m_lights.size(); ++lightIndex)
	{
		if (scene->m_lights[lightIndex]->m_light->m_isShadowCasting)
		{
			RenderShadowCastingObjectsForLight(scene->m_lights[lightIndex], scene);
		}
	}

	for(Camera* camera : scene->m_cameras)
	{
		RenderSceneForCamera(camera, scene);
	}

	scene = nullptr;
}

void ForwardRenderingPath::RenderSceneForCamera(Camera* camera, RenderScene* scene)
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetCamera(camera);
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	if (camera->m_skybox != nullptr)
	{
		theRenderer->DrawSkybox(camera->m_skybox);
	}
	
	std::vector<DrawCallData> drawCalls;

	for (Renderable* renderable : scene->m_renderables)
	{
		DrawCallData drawCall;

		drawCall.m_material = renderable->GetMaterial();
		for (int meshIndex = 0; meshIndex < (int)renderable->m_meshes.size(); ++meshIndex)
		{
			drawCall.m_meshes.push_back(renderable->GetMesh(meshIndex));
		}
		drawCall.m_model = renderable->m_transform->GetWorldMatrix();
		drawCall.m_sortingLayer = renderable->GetRenderSortLayer();
		drawCall.m_renderType = renderable->GetMaterial()->GetRenderQueueType();

		if(renderable->m_material->GetDoesUseLighting())
		{
			drawCall.m_contributingLights = scene->GetMostContributingLights(renderable->m_transform->GetWorldMatrix().GetPosition());
		}	

		drawCalls.push_back(drawCall);
	}

	SortDrawsBySortOrder(drawCalls);
	SortDrawsByRenderQueue(drawCalls);
	SortDrawsByCameraDistance(drawCalls, camera->m_transform->GetWorldPosition());

	for (DrawCallData drawCall : drawCalls)
	{	
		for (int meshIndex = 0; meshIndex < (int)drawCall.m_meshes.size(); ++meshIndex)
		{
			if(drawCall.m_meshes[meshIndex] != nullptr)
			{
				theRenderer->SetLightBufferFromArray(drawCall.m_contributingLights);		
				theRenderer->BindMaterial(drawCall.m_material);

				int handle = drawCall.m_material->GetShaderProgramHandle();
				if(handle > -1)
				{
					Vector3 cameraPosition = camera->m_transform->GetWorldPosition();
					theRenderer->SetVector3Uniform(handle, "EYE_POSITION", cameraPosition);
				}	

				theRenderer->DrawMesh(drawCall.m_meshes[meshIndex], drawCall.m_model);
			}	
		}
	}

	theRenderer = nullptr;
	camera = nullptr;
	scene = nullptr;
}

//sorts the array by sortingLayer
void ForwardRenderingPath::SortDrawsBySortOrder(std::vector<DrawCallData> outDrawCalls)
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

//sorts the array rendertype if layers are same
void ForwardRenderingPath::SortDrawsByRenderQueue(std::vector<DrawCallData> outDrawCalls)
{
	for(int i = 0; i < (int)outDrawCalls.size(); i++)
	{
		for(int j = i + 1; j < (int)outDrawCalls.size(); j++)
		{
			if(outDrawCalls[j].m_sortingLayer > outDrawCalls[i].m_sortingLayer)
			{
				break;
			}

			//make sure the drawcalls are on the same layer first
			if(outDrawCalls[j].m_sortingLayer == outDrawCalls[i].m_sortingLayer)
			{
				if(outDrawCalls[j].m_renderType > outDrawCalls[i].m_renderType)
				{				
					std::swap(outDrawCalls[j], outDrawCalls[i]);
				}
			}			
		}
	}
}

//sorts the array by camera Z distance if layers and rendertypes are same
void ForwardRenderingPath::SortDrawsByCameraDistance(std::vector<DrawCallData> outDrawCalls, const Vector3& cameraPosition)
{
	for(int i = 0; i < (int)outDrawCalls.size(); i++)
	{
		for(int j = i + 1; j < (int)outDrawCalls.size(); j++)
		{
			if(outDrawCalls[j].m_sortingLayer > outDrawCalls[i].m_sortingLayer)
			{
				break;
			}

			if(outDrawCalls[j].m_renderType > outDrawCalls[i].m_renderType)
			{
				break;
			}

			if(outDrawCalls[j].m_sortingLayer == outDrawCalls[i].m_sortingLayer && outDrawCalls[j].m_renderType == outDrawCalls[i].m_renderType)
			{
				if(GetDistanceSquared(cameraPosition, outDrawCalls[j].m_model.GetPosition()) > GetDistanceSquared(cameraPosition, outDrawCalls[i].m_model.GetPosition()))
				{				
					std::swap(outDrawCalls[j], outDrawCalls[i]);
				}
			}
		}
	}
}

void ForwardRenderingPath::RenderShadowCastingObjectsForLight(LightObject* light, RenderScene* scene)
{
	Camera camera;

	//switch(light->)
}