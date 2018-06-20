#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Tank.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Bullet.hpp"


PlayingState::~PlayingState()
{
	delete(m_playerTank);
	m_playerTank = nullptr;

	delete(m_terrain);
	m_terrain = nullptr;

	delete(m_renderScene);
	m_renderScene = nullptr;

	delete(m_renderScene2D);
	m_renderScene2D = nullptr;
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	m_camera->m_transform->TranslatePosition(Vector3(0.f, 3.f, -10.f));

	//position camera behind player	
	m_camera->m_skybox = new Skybox("Data/Images/galaxy2.png");
	m_renderScene->AddCamera(m_camera);

	m_renderScene2D->AddCamera(m_uiCamera);

	// add directional light =========================================================================================
	Rgba lightColor = Rgba::WHITE;
	LightObject* directionalLight = new LightObject("directionalLight", LIGHT_TYPE_DIRECTIONAL_LIGHT, lightColor, 0.8f, Vector3(1.f, 0.f, 0.f), 1.f, 360.f, 360.f);

	directionalLight->m_transform->TranslatePosition(Vector3(500.f, 50.f, 0.f));
	directionalLight->m_renderScene = m_renderScene;

	Renderable* lightRenderable = new Renderable();
	directionalLight->m_transform->AddChildTransform(lightRenderable->m_transform);

	meshBuilder.CreateUVSphere( Vector3::ZERO, 20.f, 15, 15, Rgba::WHITE);	
	lightRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	lightRenderable->SetMaterial(new Material());
	lightRenderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	lightRenderable->GetMaterial()->SetTexture(lightRenderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	lightRenderable->GetMaterial()->SetSampler(lightRenderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);

	directionalLight->AddRenderable(lightRenderable);
	directionalLight->m_transform->SetLocalRotation(Vector3(30.f, -90.f, 0.f));
	directionalLight->UpdateLightFromWorldTransform();

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
	m_playerTank->Initialize();

	// add terrain =========================================================================================
	m_terrain = new Terrain("terrain", Vector3(0.f, 0.f, 0.f), AABB2(-50, -50, 50.f, 50.f), 2.f, 10.f, "Data/Images/terrain.jpg");
	m_terrain->GenerateMeshFromHeightMap();

	for (int renderableIndex = 0; renderableIndex < (int)m_terrain->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_terrain->m_renderables[renderableIndex]);
	}


	theRenderer = nullptr;	
}

void PlayingState::Update(float deltaSeconds)
{ 
	// tank update =============================================================================
	m_playerTank->Update(deltaSeconds);

	// update bullets =========================================================================================
	for (int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); ++bulletIndex)
	{
		m_bullets[bulletIndex]->Update(deltaSeconds);
	}
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
	for (int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); bulletIndex++)
	{
		if (!m_bullets[bulletIndex]->IsAlive())
		{
			delete(m_bullets[bulletIndex]);
			m_bullets[bulletIndex] = nullptr;

			m_bullets.erase(m_bullets.begin() + bulletIndex);
			bulletIndex--;
		}
	}
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();

	m_playerTank->UpdateFromInput(deltaSeconds);


	//cleanup
	theInput = nullptr;

	return deltaSeconds; //new deltaSeconds
}

void PlayingState::SpawnBullet(const Vector3 & startingPosition, const Vector3& startingRotation)
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Bullet* bullet = new Bullet(startingPosition, startingRotation);
	Renderable* bulletRenderable = new Renderable();

	bullet->m_gameState = this;
	bullet->AddRenderable(bulletRenderable);
	bullet->m_transform->AddChildTransform(bulletRenderable->m_transform);

	Rgba bulletTint = Rgba::LIGHT_RED_TRANSPARENT;

	bullet->m_renderScene = m_renderScene;

	//create bullet mesh and material
	meshBuilder.CreateStarQuads3D(Vector3::ZERO, Vector3(1.f, 1.f, 3.f), Rgba::WHITE);	
	bulletRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	bulletRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("bullet")));	
	bulletRenderable->GetMaterial()->SetProperty("TINT", Rgba::ConvertToVector4(bulletTint));

	//add light to lists
	m_renderScene->AddRenderable(bulletRenderable);
	m_renderScene->AddLight(bullet->m_light);

	m_bullets.push_back(bullet);

	bulletRenderable = nullptr;
	bullet = nullptr;
	theRenderer = nullptr;
}
