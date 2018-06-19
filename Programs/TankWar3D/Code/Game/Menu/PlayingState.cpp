#include "Game\Menu\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Tank.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\StringUtils.hpp"


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

	m_camera->m_transform->TranslatePosition(Vector3(0.f, 3.f, -10.f));

	//position camera behind player	
	m_camera->m_skybox = new Skybox("Data/Images/galaxy2.png");
	m_renderScene->AddCamera(m_camera);

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

	m_playerTank->m_baseDimensions = Vector3(1.0f, 0.5f, 2.0f);

	Renderable* tankRenderable = new Renderable();
	meshBuilder.LoadObjectFromFile("Data/Model/claire_car/RallyFighter.obj");
	tankRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	tankRenderable->SetMaterial(Renderer::GetInstance()->CreateOrGetMaterial("rallyfighter"));
	tankRenderable->m_transform->SetLocalScale(tankRenderable->m_transform->GetLocalScale() * 0.01);

	m_playerTank->AddRenderable(tankRenderable);
	m_playerTank->m_tankBodyTransform->AddChildTransform(tankRenderable->m_transform);
	m_playerTank->m_breadCrumbTimer = new Stopwatch(Game::GetInstance()->m_gameClock);
	m_playerTank->m_breadCrumbTimer->SetTimer(0.5f);

	//add turret to player tank
	Turret* tankTurret = new Turret();
	Renderable* turretRenderable = new Renderable();

	meshBuilder.CreateUVSphere(Vector3::ZERO, 0.25f, 20, 20, Rgba::WHITE);
	meshBuilder.CreateCube(Vector3(0.f, 0.f, 0.5f), Vector3(0.25f, 0.25f, 1.f), Rgba::WHITE);
	turretRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	turretRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("rallyfighter")));
	tankTurret->AddRenderable(turretRenderable);
	tankTurret->m_transform->AddChildTransform(turretRenderable->m_transform);
	tankTurret->m_transform->SetLocalPosition(Vector3(-0.15f, 1.75f, -0.5f));
	m_playerTank->m_turret = tankTurret;
	
	//cleanup tank turret and renderable
	turretRenderable = nullptr;
	tankTurret = nullptr; 
	

	m_playerTank->m_tankBodyTransform->AddChildTransform(m_playerTank->m_turret->m_transform);


	//add tank to lists
	for (int renderableIndex = 0; renderableIndex < (int)m_playerTank->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_playerTank->m_renderables[renderableIndex]);
	}

	for (int renderableIndex = 0; renderableIndex < (int)m_playerTank->m_turret->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_playerTank->m_turret->m_renderables[renderableIndex]);
	}

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

	UpdateTarget(deltaSeconds);
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

void PlayingState::UpdateTarget(float deltaSeconds)
{
	RayCastHit3 raycastResult = RaycastFromCamera(deltaSeconds);

	// update tank turret to look at to result location =========================================================================================
	Matrix44 turretWorld = m_playerTank->m_turret->m_transform->GetWorldMatrix();
	Vector3 turretWorldUp = turretWorld.GetUp();
	Matrix44 turretLookAt = turretWorld.LookAt(turretWorld.GetPosition(), raycastResult.position, turretWorldUp);

	Matrix44 worldToTank = m_playerTank->m_transform->GetWorldMatrix().GetInverse();
	worldToTank.Append(turretLookAt);

	Vector3 rotation = Vector3(worldToTank.GetRotation().x, worldToTank.GetRotation().y, 0.f);
	m_playerTank->m_turret->m_transform->SetLocalRotation(rotation); //this is the final turret look at

	// render debug position =========================================================================================
	Vector3 raycastRenderStartPosition = m_playerTank->m_transform->GetLocalPosition() + (m_playerTank->m_transform->GetWorldUp());

	DebugRender::GetInstance()->CreateDebugLine(m_playerTank->m_transform->GetWorldPosition(), raycastResult.position, Rgba::RED, Rgba::RED, 0.f, 1, m_camera);
	DebugRender::GetInstance()->CreateDebugCube(raycastResult.position, Vector3(1.f, 1.f, 1.f), Rgba::RED, Rgba::RED, 0.f, 1, m_camera);
}

RayCastHit3 PlayingState::RaycastFromCamera(float deltaSeconds)
{
	RayCastHit3 raycast;

	Vector3 cameraForward = m_camera->m_transform->GetWorldForward();
//	DebugRender::GetInstance()->CreateDebugCube(m_camera->m_transform->GetWorldPosition() + (10.f *  m_camera->m_transform->GetWorldForward()), Vector3::ONE, Rgba::BLUE, Rgba::BLUE, 0.f, 1, m_camera);

	Vector3 position = m_camera->m_transform->GetWorldPosition();
	Ray3 ray = Ray3(m_camera->m_transform->GetWorldPosition() + cameraForward * 5.f, cameraForward);

	float currentDistance = 0.f;
	float maxDistance = 50.f;

	Vector3 currentPosition = ray.start;

	while (currentDistance < maxDistance)
	{
		currentDistance += deltaSeconds;

		currentPosition = ray.Evaluate(currentDistance);

		float heightAtPosition = m_terrain->GetHeightAtPositionXZ(Vector2(currentPosition.x, currentPosition.z));
		if (currentPosition.y <= heightAtPosition)
		{
			raycast.hit = true;
			raycast.position = Vector3(currentPosition.x, heightAtPosition, currentPosition.z);
			return raycast;
		}
	}

	if (raycast.hit == false)
	{
		raycast.position = currentPosition;
	}

	return raycast;
}