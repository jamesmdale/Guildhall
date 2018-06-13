#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Tank.hpp"

PlayingState::~PlayingState()
{
	delete(m_playerTank);
	m_playerTank = nullptr;

	delete(m_terrain);
	m_terrain = nullptr;
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	//position camera behind player
	m_camera->Translate(Vector3(0.f, 2.f, -10.f));
	m_camera->m_skybox = new Skybox("Data/Images/galaxy2.png");

	m_renderScene->AddCamera(m_camera);

	// add light =========================================================================================
	Rgba lightColor = Rgba::WHITE;
	LightObject* directionalLight = new LightObject("directionalLight", LIGHT_TYPE_DIRECTIONAL_LIGHT, lightColor, 0.8f, Vector3(1.f, 0.f, 0.f), 1.f, 360.f, 360.f);

	directionalLight->m_transform->TranslatePosition(Vector3(500.f, 50.f, 0.f));
	directionalLight->m_renderScene = m_renderScene;

	Renderable* lightRenderable = new Renderable();
	meshBuilder.CreateUVSphere( Vector3::ZERO, 20.f, 15, 15, Rgba::WHITE);	
	lightRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	lightRenderable->SetMaterial(new Material());
	lightRenderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	lightRenderable->GetMaterial()->SetTexture(lightRenderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	lightRenderable->GetMaterial()->SetSampler(lightRenderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);

	directionalLight->AddRenderable(lightRenderable);
	directionalLight->m_transform->SetLocalRotation(Vector3(30.f, -90.f, 0.f));
	directionalLight->UpdateRenderableFromTransform();
	directionalLight->UpdateLightFromWorldTransform();

	directionalLight->PreRender();

	//add light to lists
	m_renderScene->AddLight(directionalLight->m_light);

	for (int renderableIndex = 0; renderableIndex < (int)directionalLight->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(directionalLight->m_renderables[renderableIndex]);
	}
	

	// add tank =========================================================================================
	m_playerTank = new Tank();
	m_playerTank->SetCamera(m_camera);
	m_playerTank->m_playingState = (PlayingState*)g_currentState;

	Renderable* tankRenderable = new Renderable();
	meshBuilder.CreateUVSphere( Vector3::ZERO, 1.f, 15, 15, Rgba::WHITE);
	tankRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	tankRenderable->SetMaterial(Renderer::GetInstance()->CreateOrGetMaterial("tank"));

	m_playerTank->AddRenderable(tankRenderable);
	m_playerTank->m_transform->TranslatePosition(Vector3(0.f, 10.f, 0.f));
	m_playerTank->m_breadCrumbTimer = new Stopwatch(Game::GetInstance()->m_gameClock);
	m_playerTank->m_breadCrumbTimer->SetTimer(0.5f);

	//add tank to lists
	for (int renderableIndex = 0; renderableIndex < (int)m_playerTank->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_playerTank->m_renderables[renderableIndex]);
	}

	// add terrain =========================================================================================
	m_terrain = new Terrain("terrain", Vector3(0.f, 0.f, 0.f), AABB2(-50, -50, 50.f, 50.f), 1.f, 10.f, "Data/Images/terrain.jpg");
	m_terrain->GenerateMeshFromHeightMap();
	m_terrain->m_transform->TranslatePosition(Vector3(0.0f, -10.0f, 0.0f));

	for (int renderableIndex = 0; renderableIndex < (int)m_terrain->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_terrain->m_renderables[renderableIndex]);
	}

	theRenderer = nullptr;	
}

void PlayingState::Update(float deltaSeconds)
{ 
	m_playerTank->Update(deltaSeconds);
}

void PlayingState::PreRender()
{
	m_playerTank->PreRender();
}

void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	Game::GetInstance()->m_forwardRenderingPath->Render(m_renderScene);

	theRenderer = nullptr;
}

void PlayingState::PostRender()
{
	//post render steps here.
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();

	m_playerTank->UpdateFromInput(deltaSeconds);

	//cleanup
	theInput = nullptr;

	return deltaSeconds; //new deltaSeconds
}

