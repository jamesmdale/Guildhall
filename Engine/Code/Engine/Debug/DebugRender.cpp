#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"

static DebugRender* g_theDebugRenderer = nullptr;

//  =============================================================================
DebugRender::DebugRender()
{

}

//  =============================================================================
void DebugRender::CreateDebugPoint(const Vector3& position, float scale, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType, Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = LINE_SEGMENT_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreatePoint(position, Rgba::WHITE, scale);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	//transform is identity for line
	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugLine(const Vector3& positionStart, const Vector3& positionEnd, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	
	
	//setting data
	renderObject->m_type = LINE_SEGMENT_RENDER_TYPE;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_depthType = depthType;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateLine(positionStart, positionEnd, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	//transform is identity for line

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugBasis(const Vector3& positionStart, float scale, float timeToLive, int depthType, Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = BASIS_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	//colors are predetermined by basis

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreateBasis(Vector3::ZERO, scale);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();
	

	renderObject->m_transform.SetLocalPosition(positionStart);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugBasis(const Matrix44& basis, const Vector3& position, float scale, float timeToLive, int depthType, Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = BASIS_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	//colors are predetermined by basis

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateBasis(basis, Vector3::ZERO, scale);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(position);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugSphere(const Vector3& center, float radius, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = SPHERE_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreateUVSphere(Vector3::ZERO, radius, 25, 25, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();


	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugWireSphere(const Vector3& center, float radius, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = WIRE_SPHERE_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateUVSphere(Vector3::ZERO, radius, 25, 25, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugWireCube(const Vector3 & center, const Vector3 & dimensions, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = WIRE_CUBE_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreateCube(Vector3::ZERO, dimensions, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();	

	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugQuad(const Vector3& center, const Vector2& dimensions, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = QUAD_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateQuad3D(Vector3::ZERO, dimensions, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugBillboardQuad(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2& dimensions, const Rgba& startColor, const Rgba& endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = QUAD_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateBillboardQuad3d(Vector3::ZERO, up, right, dimensions, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugCube(const Vector3 & center, const Vector3 & dimensions, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType,  Camera* camera)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	if(camera == nullptr)
	{
		renderObject->m_camera = theRenderer->m_defaultCamera;
	}
	else
	{
		renderObject->m_camera = camera;
	}	

	//setting data
	renderObject->m_type = CUBE_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreateCube(Vector3::ZERO, dimensions, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(center);

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugQuad2D(const Vector2 & center, const Vector2 & dimensions, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();
	
	renderObject->m_camera = m_debugCamera;	

	//setting data
	renderObject->m_type = QUAD_2D_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateQuad2D(Vector2::ZERO, dimensions, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	renderObject->m_transform.SetLocalPosition(Vector3(center.x, center.y, 0.f));

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugLine2D(const Vector2 & positionStart, const Vector2 & positionEnd, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	renderObject->m_camera = m_debugCamera;	

	//setting data
	renderObject->m_type = LINE_2D_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateLine2D(positionStart, positionEnd, Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	//line has identity model matrix in transform

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugCrosshair2D(const Vector2 & center, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	renderObject->m_camera = m_debugCamera;	
				
	//setting data
	renderObject->m_type = CROSSHAIR_2D_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;
	
	meshBuilder.CreateLine2D(center + Vector2(0.f, 10.f), center + Vector2(0.f, -10.f), Rgba::WHITE);
	meshBuilder.CreateLine2D(center + Vector2(10.f, 0.f), center + Vector2(-10.f, 0.f), Rgba::WHITE);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();

	//line has identity model matrix in transform

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::CreateDebugText2D(const Vector2 & center, float cellHeight, float aspectScale, std::string text, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType)
{
	DebugRenderObject* renderObject = new DebugRenderObject();
	Renderer* theRenderer = Renderer::GetInstance();

	renderObject->m_camera = m_debugCamera;	

	//setting data
	renderObject->m_type = TEXT_2D_RENDER_TYPE;
	renderObject->m_depthType = depthType;
	renderObject->m_totalTimeToLive = timeToLive;
	renderObject->m_remainingTimeToLive = timeToLive;
	renderObject->m_startColor = startColor;
	renderObject->m_endColor = endColor;

	//mesh creation logic
	MeshBuilder meshBuilder;

	meshBuilder.CreateText2D(center, cellHeight, aspectScale, text);
	renderObject->m_mesh = meshBuilder.CreateMesh<VertexPCU>();	

	m_debugObjects.push_back(renderObject);
	theRenderer = nullptr;
}

//  =============================================================================
DebugRender* DebugRender::CreateInstance() 
{
	if (g_theDebugRenderer == nullptr) 
	{
		g_theDebugRenderer = new DebugRender(); 
	}
	return g_theDebugRenderer; 
}

//  =============================================================================
void DebugRender::EndFrame()
{
	for(int renderObjectIndex = 0; renderObjectIndex < (int)m_debugObjects.size(); renderObjectIndex++)
	{
		if(m_debugObjects[renderObjectIndex]->m_isDrawFinished)
		{
			delete(m_debugObjects[renderObjectIndex]);
			m_debugObjects[renderObjectIndex] = nullptr;

			m_debugObjects.erase(m_debugObjects.begin() + renderObjectIndex);
			renderObjectIndex--;
		}
	}
}

//  =============================================================================
void DebugRender::Startup()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();

	m_isEnabled = true;

	m_debugCamera = new Camera();
	m_debugCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_debugCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_debugCamera->SetView(Matrix44::IDENTITY);

	RegisterCommand("toggle_debug", CommandRegistration(ToggleDebug, ": Use to toggle debug rendering on or off.", "Toggled debug rendering"));
	RegisterCommand("clear_debug", CommandRegistration(ClearDebug, ": Use to clear all debug render objects", "Cleared debug renderer!"));

	m_debugShader = new Shader();
	m_debugShader->SetProgram(theRenderer->CreateOrGetShaderProgramFromPath("Data/Shaders/debug"));

	theWindow = nullptr;
	theRenderer = nullptr;
}

//  =============================================================================
void DebugRender::Shutdown()
{
	delete(g_theDebugRenderer);
	g_theDebugRenderer = nullptr;
}

//  =============================================================================
void DebugRender::Update(float deltaSeconds)
{
	if((int)m_debugObjects.size() > 0)
	{
		for(int debugObjectIndex = 0; debugObjectIndex < (int)m_debugObjects.size(); debugObjectIndex++)
		{
			m_debugObjects[debugObjectIndex]->Update(deltaSeconds);
		}
	}
}

//  =============================================================================
void DebugRender::Render()
{
	if((int)m_debugObjects.size() > 0)
	{
		for(int debugObjectIndex = 0; debugObjectIndex < (int)m_debugObjects.size(); debugObjectIndex++)
		{
			m_debugObjects[debugObjectIndex]->Render();
		}
	}
}

//  =============================================================================
void DebugRender::ToggleDebugRenderer()
{
	m_isEnabled = !m_isEnabled;
}

//  =============================================================================
DebugRender* DebugRender::GetInstance()
{
	return g_theDebugRenderer; 
}

//  =============================================================================
void ToggleDebug(Command & cmd)
{	
	UNUSED(cmd);
	DebugRender::GetInstance()->ToggleDebugRenderer();


	if(DebugRender::GetInstance()->IsEnabled())
	{
		DevConsolePrintf("Debug renderer enabled!");
	}
	else
	{
		DevConsolePrintf("Debug renderer disabled!");
	}
}

//  =============================================================================
void ClearDebug(Command & cmd)
{
	DebugRender::GetInstance()->ClearDebugRenderer();
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
}
