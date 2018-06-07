#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"

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

	//create directional light
	Rgba lightColor = Rgba::WHITE;
	LightObject* directionalLight = new LightObject("directionalLight", LIGHT_TYPE_DIRECTIONAL_LIGHT, lightColor, 0.8f, Vector3(1.f, 0.f, 0.f), 1.f, 360.f, 360.f);

	directionalLight->m_transform->SetLocalPosition(Vector3(1000.f, 0.f, 0.f));
	directionalLight->m_renderScene = m_renderScene;

	meshBuilder.CreateUVSphere( Vector3::ZERO, 20.f, 15, 15, Rgba::WHITE);
	directionalLight->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexPCU>());
	directionalLight->m_renderable->SetMaterial(new Material());
	directionalLight->m_renderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	directionalLight->m_renderable->GetMaterial()->SetTexture(directionalLight->m_renderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	directionalLight->m_renderable->GetMaterial()->SetSampler(directionalLight->m_renderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);

	directionalLight->m_transform->SetLocalRotation(Vector3(0.f, -90.f, 0.f));
	directionalLight->UpdateRenderableFromTransform();
	directionalLight->UpdateLightFromWorldTransform();

	directionalLight->PreRender();

	//add light to lists
	m_renderScene->AddLight(directionalLight->m_light);
	m_renderScene->AddRenderable(directionalLight->m_renderable);

	//add player tank renderable
	m_playerTank = new Tank();
	m_playerTank->SetCamera(m_camera);

	//create ship game object
	meshBuilder.CreateUVSphere( Vector3::ZERO, 1.f, 15, 15, Rgba::WHITE);
	m_playerTank->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexLit>());	
	m_playerTank->m_renderable->SetMaterial(Renderer::GetInstance()->CreateOrGetMaterial("tank"));
	m_playerTank->m_transform->SetLocalPosition(Vector3::ZERO);
	m_playerTank->m_breadCrumbTimer = new Stopwatch(Game::GetInstance()->m_gameClock);
	m_playerTank->m_breadCrumbTimer->SetTimer(0.5f);

	//add tank to lists
	m_renderScene->AddRenderable(m_playerTank->m_renderable);	

	//add terrain
	/*m_terrain = new Terrain("terrain", Vector3(0.f, -1.f, 0.f), AABB2(Vector2::ZERO, 50.f, 50.f), 1.f, "Data/Images/heightmap.png");
	m_renderScene->AddRenderable(m_terrain);	*/

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

