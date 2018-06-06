#include "Engine\Renderer\RenderScene2D.hpp"



RenderScene2D::RenderScene2D()
{
}


RenderScene2D::~RenderScene2D()
{
	//clear renderables
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); renderableIndex++)
	{
		m_renderables[renderableIndex] = nullptr;
	}
	m_renderables.clear();

	//clear cameras
	for (int cameraIndex = 0; cameraIndex < (int)m_cameras.size(); cameraIndex++)
	{
		m_cameras[cameraIndex] = nullptr;
	}
	m_cameras.clear();
}

void RenderScene2D::RemoveRenderable(Renderable2D * renderableToRemove)
{
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); renderableIndex++)
	{
		if(m_renderables[renderableIndex] == renderableToRemove)
		{
			m_renderables.erase(m_renderables.begin() + renderableIndex);
			renderableIndex--;
		}
	}
}

void RenderScene2D::RemoveCamera(Camera* cameraToRemove)
{
	for (int cameraIndex = 0; cameraIndex < (int)m_cameras.size(); cameraIndex++)
	{
		if(m_cameras[cameraIndex] == cameraToRemove)
		{
			m_cameras.erase(m_cameras.begin() + cameraIndex);
			cameraIndex--;
		}
	}
}
