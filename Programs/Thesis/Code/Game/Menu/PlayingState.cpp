#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Profiler\ProfilerConsole.hpp"

bool isPlayerAlive = true;
bool isVictory = false;

SoundPlaybackID musicID;

PlayingState::~PlayingState()
{
	delete(m_renderScene);
	m_renderScene = nullptr;

	delete(m_renderScene2D);
	m_renderScene2D = nullptr;
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	//init scenes and ui
	m_renderScene = new RenderScene();
	m_renderScene2D = new RenderScene2D();

	m_camera->m_transform->TranslatePosition(Vector3(0.f, 3.f, -10.f));

	//position camera behind player	
	m_camera->m_skybox = new Skybox("Data/Images/galaxy2.png");
	m_renderScene->AddCamera(m_camera);

	m_renderScene2D->AddCamera(m_uiCamera);

	// add directional light =========================================================================================
	Rgba lightColor = Rgba::WHITE;
	LightObject* directionalLight = new LightObject("directionalLight", LIGHT_TYPE_DIRECTIONAL_LIGHT, lightColor, 0.8f, Vector3(1.f, 0.f, 0.f), 1.f, 360.f, 360.f);

	directionalLight->m_transform->TranslatePosition(Vector3(-10.f, 25.f, -10.f));
	directionalLight->m_renderScene = m_renderScene;

	Renderable* lightRenderable = new Renderable();
	directionalLight->m_transform->AddChildTransform(lightRenderable->m_transform);
	directionalLight->m_light->m_isShadowCasting = true;

	meshBuilder.CreateUVSphere( Vector3::ZERO, 5.f, 5.f, 5.f, Rgba::WHITE);	
	lightRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	lightRenderable->SetMaterial(new Material());
	lightRenderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	lightRenderable->GetMaterial()->SetTexture(lightRenderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	lightRenderable->GetMaterial()->SetSampler(lightRenderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);

	directionalLight->AddRenderable(lightRenderable);
	directionalLight->m_transform->SetLocalRotation(Vector3(30.f, -90.f, 0.f));
	directionalLight->UpdateLightFromWorldTransform();

	lightRenderable = nullptr;

	//add light to lists
	m_renderScene->AddLightObject(directionalLight);

	for (int renderableIndex = 0; renderableIndex < (int)directionalLight->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(directionalLight->m_renderables[renderableIndex]);
	}	


	theRenderer = nullptr;	

	m_isInitialized = true;
}

void PlayingState::Update(float deltaSeconds)
{ 
	
}

void PlayingState::PreRender()
{

}

void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	Game::GetInstance()->m_forwardRenderingPath->Render(m_renderScene);
	Game::GetInstance()->m_forwardRenderingPath2D->Render(m_renderScene2D);

	theRenderer = nullptr;
}

void PlayingState::PostRender()
{
	
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	//if we are transitioning, don't accept input
	if (GetTransitionMenuState() != nullptr)
	{
		return deltaSeconds;
	}

	InputSystem* theInput = InputSystem::GetInstance();

	if (!ProfilerConsole::GetInstance()->m_doesHaveInputPriority)
	{
		
	}	

	//cleanup
	theInput = nullptr;

	return deltaSeconds; //new deltaSeconds
}

void PlayingState::TransitionIn(float secondsTransitioning)
{
	Initialize();
	s_isFinishedTransitioningIn = true;
}

void PlayingState::TransitionOut(float secondsTransitioning)
{
	ResetState();
	s_isFinishedTransitioningOut = true;
}

void PlayingState::ResetState()
{
	m_camera->m_transform->ResetPositionData();


	delete(m_renderScene);
	m_renderScene = nullptr;

	delete(m_renderScene2D);
	m_renderScene2D = nullptr;	

	isVictory = false;
}
