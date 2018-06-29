#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Tank.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Bullet.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Game\Swarmer.hpp"
#include "Game\Spawner.hpp"
#include "Engine\Audio\AudioSystem.hpp"

bool isPlayerAlive = true;
bool isVictory = false;

SoundPlaybackID musicID;

PlayingState::~PlayingState()
{
	delete(m_playerTank);
	m_playerTank = nullptr;

	delete(m_waterPlane);
	m_waterPlane = nullptr;

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

	//init scenes and ui
	m_renderScene = new RenderScene();
	m_renderScene2D = new RenderScene2D();
	m_ui = new TankUI();

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

	// add tank =========================================================================================
	m_playerTank = new Tank();
	m_playerTank->SetCamera(m_camera);
	m_playerTank->m_gameState = this;
	m_playerTank->Initialize();

	// add terrain =========================================================================================
	if (m_terrain == nullptr)
	{
		m_terrain = new Terrain("terrain", Vector3(0.f, 0.f, 0.f), AABB2(-100, -100, 100.f, 100.f), 1.f, 10.f, "Data/Images/terrain.jpg");
		m_terrain->GenerateMeshFromHeightMap();
	}

	m_terrain->m_renderScene = m_renderScene;
	for (int renderableIndex = 0; renderableIndex < (int)m_terrain->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_terrain->m_renderables[renderableIndex]);
	}

	// add water object =============================================================================
	m_waterPlane = new GameObject();

	Vector2 terrainDimensions = Vector2(m_terrain->m_worldBounds.GetDimensions().x, m_terrain->m_worldBounds.GetDimensions().z);

	Renderable* waterRenderable = new Renderable();
	Vector2 center = Vector2(m_terrain->m_worldBounds.GetCenter().x, m_terrain->m_worldBounds.GetCenter().z);
	Vector2 dimensions = Vector2(m_terrain->m_worldBounds.GetDimensions().x, m_terrain->m_worldBounds.GetDimensions().z);
	meshBuilder.CreateQuad2D(Vector2::ZERO, terrainDimensions, Rgba::WHITE);	
	waterRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	waterRenderable->SetMaterial(Material::Clone(theRenderer->CreateOrGetMaterial("water")));
	waterRenderable->m_transform->AddRotation(Vector3(90.f, 0.f, 0.f));
	waterRenderable->m_transform->SetLocalPosition(Vector3(0.f, 0.5f, 0.f));

	m_waterPlane->m_transform->AddChildTransform(waterRenderable->m_transform);

	m_waterPlane->AddRenderable(waterRenderable);
	m_waterPlane->m_renderScene = m_renderScene;
	for (int renderableIndex = 0; renderableIndex < (int)m_waterPlane->m_renderables.size(); ++renderableIndex)
	{
		m_waterPlane->m_renderScene->AddRenderable(m_waterPlane->m_renderables[renderableIndex]);
	}

	waterRenderable = nullptr;

	// add spawners =========================================================================================
	for (int spawnerIndex = 0; spawnerIndex < g_startingNumSpawners; ++spawnerIndex)
	{
		Spawner* spawner = new Spawner();
		spawner->m_gameState = this;
		spawner->m_renderScene = m_renderScene;
		spawner->Initialize();

		float xPosition = GetRandomFloatInRange(m_terrain->m_worldBounds.mins.x, m_terrain->m_worldBounds.maxs.x);
		float zPosition = GetRandomFloatInRange(m_terrain->m_worldBounds.mins.z, m_terrain->m_worldBounds.maxs.z);

		float height = m_terrain->GetHeightAtPositionXZ(Vector2(xPosition, zPosition));

  		spawner->m_transform->SetLocalPosition(Vector3(xPosition, height + (g_spawnerDimensions.y * 0.5f) - 1.f, zPosition));

		m_spawners.push_back(spawner);

		spawner = nullptr;
	}

	// UI creation =========================================================================================
	m_ui->m_renderScene = m_renderScene2D;
	m_ui->Initialize();

	AudioSystem* audio = AudioSystem::GetInstance();
	musicID = audio->PlaySound(audio->CreateOrGetSound("Data/Audio/Panoramic.mp3"), true, 0.75f, 0.f, 1.f, false);

	isVictory = false;

	audio = nullptr;
	theRenderer = nullptr;	

	m_isInitialized = true;
}

void PlayingState::Update(float deltaSeconds)
{ 
	//if player is dead, don't update any game elements
	if (isPlayerAlive && !isVictory)
	{
		// tank update =============================================================================
		m_playerTank->Update(deltaSeconds);

		// update spawner =========================================================================================
		for (int spawnerIndex = 0; spawnerIndex < (int)m_spawners.size(); ++spawnerIndex)
		{
			m_spawners[spawnerIndex]->Update(deltaSeconds);
		}

		// update swarmer =========================================================================================
		for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); ++swarmerIndex)
		{
			m_swarmers[swarmerIndex]->Update(deltaSeconds);
		}

		// update bullets =========================================================================================
		for (int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); ++bulletIndex)
		{
			m_bullets[bulletIndex]->Update(deltaSeconds);
		}
	}

	// copy new tank information to tank ui =========================================================================================
	RefreshTankUI();

	//update ui information
	m_ui->Update(deltaSeconds);
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

	for (int spawnerIndex = 0; spawnerIndex < (int)m_spawners.size(); spawnerIndex++)
	{
		if (!m_spawners[spawnerIndex]->IsAlive())
		{
			delete(m_spawners[spawnerIndex]);
			m_spawners[spawnerIndex] = nullptr;

			m_spawners.erase(m_spawners.begin() + spawnerIndex);
			spawnerIndex--;
		}
	}

	for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); swarmerIndex++)
	{
		if (!m_swarmers[swarmerIndex]->IsAlive())
		{
			//remove swarmer from gamestate list
			Swarmer* swarmer = m_swarmers[swarmerIndex];
			RemoveDeadSwarmer(m_swarmers[swarmerIndex]);
			delete(swarmer);
			swarmer = nullptr;
			swarmerIndex--;
		}
	}

	if (!m_playerTank->IsAlive())
	{
		if (isPlayerAlive == true)
		{
			isPlayerAlive = false;
			m_respawnTimer->Reset();
		}
	}

	if (m_spawners.size() == 0 && m_swarmers.size() == 0)
	{

		isVictory = true;
	}
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	//if we are transitioning, don't accept input
	if (GetTransitionMenuState() != nullptr)
	{
		return deltaSeconds;
	}

	InputSystem* theInput = InputSystem::GetInstance();

	//if tank is dead, it no longer has input.
	if (isPlayerAlive && !isVictory)
	{
		m_playerTank->UpdateFromInput(deltaSeconds);
	}
	else if (m_respawnTimer->HasElapsed())
	{
		if(theInput->WasKeyJustPressed(theInput->KEYBOARD_SPACE))
		{
			isPlayerAlive = true;
			RespawnTank();
		}
	}
	
	if (isVictory == true)
	{
		if (theInput->WasKeyJustPressed(theInput->KEYBOARD_SPACE))
		{
			isVictory = false;
			
			//after you are finished loading
			GameState* state = GetMenuStateFromGlobalListByType(MAIN_MENU_STATE);
			GUARANTEE_OR_DIE(state != nullptr, "LOADING STATE TRANSITION: PLAYING STATE NOT FOUND");

			AudioSystem::GetInstance()->StopSound(musicID);

			TransitionMenuStates(GetMenuStateFromGlobalListByType(MAIN_MENU_STATE));
		}	
	}

	//kill all enemies
	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_K))
	{
		for (int spawnerIndex = 0; spawnerIndex < (int)m_spawners.size(); ++spawnerIndex)
		{
			m_spawners[spawnerIndex]->m_health = 0;
		}

		for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); ++swarmerIndex)
		{
			m_swarmers[swarmerIndex]->m_health = 0;
		}
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

	//delete bullets
	for (int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); bulletIndex++)
	{
		delete(m_bullets[bulletIndex]);
		m_bullets[bulletIndex] = nullptr;

		m_bullets.erase(m_bullets.begin() + bulletIndex);
		bulletIndex--;		
	}
	m_bullets.clear();

	//delete spawners
	for (int spawnerIndex = 0; spawnerIndex < (int)m_spawners.size(); spawnerIndex++)
	{
		delete(m_spawners[spawnerIndex]);
		m_spawners[spawnerIndex] = nullptr;

		m_spawners.erase(m_spawners.begin() + spawnerIndex);
		spawnerIndex--;		
	}
	m_spawners.clear();

	//delete swarmers
	for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); swarmerIndex++)
	{
		//remove swarmer from gamestate list
		Swarmer* swarmer = m_swarmers[swarmerIndex];
		RemoveDeadSwarmer(m_swarmers[swarmerIndex]);
		delete(swarmer);
		swarmer = nullptr;
		swarmerIndex--;
	}
	m_swarmers.clear();

	delete(m_playerTank);
	m_playerTank = nullptr;

	delete(m_ui);
	m_ui = nullptr;

	//delete(m_terrain);
	//m_terrain = nullptr;

	delete(m_renderScene);
	m_renderScene = nullptr;

	delete(m_renderScene2D);
	m_renderScene2D = nullptr;	

	isVictory = false;
}

void PlayingState::SpawnBullet(const Vector3 & startingPosition, const Vector3& startingRotation)
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Bullet* bullet = new Bullet(startingPosition, startingRotation);
	Renderable* bulletRenderable = new Renderable();

	bullet->m_gameState = (PlayingState*)g_currentState;
	bullet->AddRenderable(bulletRenderable);
	bullet->m_transform->AddChildTransform(bulletRenderable->m_transform);

	Rgba bulletTint = Rgba::LIGHT_RED_TRANSPARENT;

	bullet->m_renderScene = m_renderScene;
	bullet->m_lightObject->m_renderScene = m_renderScene;

	//create bullet mesh and material
	meshBuilder.CreateStarQuads3D(Vector3::ZERO, Vector3(1.f, 1.f, 3.f), Rgba::WHITE);	
	bulletRenderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	bulletRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("bullet")));	
	bulletRenderable->GetMaterial()->SetProperty("TINT", Rgba::ConvertToVector4(bulletTint));

	//add light to lists
	m_renderScene->AddRenderable(bulletRenderable);
	m_renderScene->AddLightObject(bullet->m_lightObject);

	AudioSystem::GetInstance()->PlaySoundFromGroup("laser");

	m_bullets.push_back(bullet);

	bulletRenderable = nullptr;
	bullet = nullptr;
	theRenderer = nullptr;
}

void PlayingState::RemoveDeadSwarmer(Swarmer * swarmer)
{
	//remove swarmer from our list. DOES NOT DELETE (just removes and resizes)
	for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); ++swarmerIndex)
	{
		if (m_swarmers[swarmerIndex] == swarmer)
		{
			m_swarmers[swarmerIndex] = nullptr;
			m_swarmers.erase(m_swarmers.begin() + swarmerIndex);
			return;
		}
	}
}

void PlayingState::RefreshTankUI()
{
	m_ui->m_tankHealthThisFrame = m_playerTank->m_heatlth;
	m_ui->m_numEnemiesThisFrame = (int)m_swarmers.size();
	m_ui->m_isPlayerAlive = isPlayerAlive;
	m_ui->m_isVictory = isVictory;
}

void PlayingState::RespawnTank()
{
	m_playerTank->m_heatlth = 100.f;
	m_playerTank->m_transform->ResetPositionData();
}
