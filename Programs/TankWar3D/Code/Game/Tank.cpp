#include "Game\Tank.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Debug\DebugRender.cpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\Raycast.hpp"


Tank::Tank()
{
	// create transforms =============================================================================
	m_cameraPivotTransform  = new Transform();
	m_tankBodyTransform  = new Transform();
	m_tankInformation = new TankUI();

	// add children to base transform =========================================================================================
	m_transform->AddChildTransform(m_cameraPivotTransform);
	m_transform->AddChildTransform(m_tankBodyTransform);
}

Tank::~Tank()
{
	delete(m_cameraPivotTransform);
	m_cameraPivotTransform = nullptr;

	delete(m_tankBodyTransform);
	m_tankBodyTransform = nullptr;	

	m_camera = nullptr;
	m_playingState = nullptr;
}

void Tank::Initialize()
{
	MeshBuilder meshBuilder;	

	m_renderScene = m_playingState->m_renderScene;

	// create tank renderable =========================================================================================
	m_baseDimensions = Vector3(1.0f, 0.5f, 2.0f);

	Renderable* tankRenderable = new Renderable();
	meshBuilder.LoadObjectFromFile("Data/Model/claire_car/RallyFighter.obj");
	tankRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	tankRenderable->SetMaterial(Renderer::GetInstance()->CreateOrGetMaterial("rallyfighter"));
	tankRenderable->m_transform->SetLocalScale(tankRenderable->m_transform->GetLocalScale() * 0.01);
	AddRenderable(tankRenderable);

	// initialize tank transforms =========================================================================================
	m_tankBodyTransform->AddChildTransform(tankRenderable->m_transform);

	// setup bread crumb timer =========================================================================================
	m_breadCrumbTimer = new Stopwatch(Game::GetInstance()->m_gameClock);
	m_breadCrumbTimer->SetTimer(0.5f);

	// add turret to tank =========================================================================================
	Turret* tankTurret = new Turret();
	Renderable* turretRenderable = new Renderable();

	meshBuilder.CreateUVSphere(Vector3::ZERO, 0.25f, 20, 20, Rgba::WHITE);
	meshBuilder.CreateCube(Vector3(0.f, 0.f, 0.5f), Vector3(0.5f, 0.5f, 1.5f), Rgba::WHITE);
	turretRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	turretRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("rallyfighter")));
	tankTurret->AddRenderable(turretRenderable);
	tankTurret->m_transform->AddChildTransform(turretRenderable->m_transform);
	tankTurret->m_transform->SetLocalPosition(Vector3(-0.15f, 1.75f, -0.5f));
	m_turret = tankTurret;

	m_tankBodyTransform->AddChildTransform(m_turret->m_transform);

	// setup tank ui =========================================================================================
	m_tankInformation->m_renderScene = m_playingState->m_renderScene2D;
	m_tankInformation->Initialize();

	// setup tank target trajectory =========================================================================================
	m_trajectory = new Renderable();

	meshBuilder.CreateLine(m_turret->m_muzzleTransform->GetWorldPosition(), m_turret->m_muzzleTransform->GetWorldPosition(), Rgba::RED);
	m_trajectory->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
	m_trajectory->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("default"))); 
	m_renderScene->AddRenderable(m_trajectory);

	// add tank renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	for (int renderableIndex = 0; renderableIndex < (int)m_turret->m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_turret->m_renderables[renderableIndex]);
	}

	// cleanup tank turret and renderable =========================================================================================
	turretRenderable = nullptr;
	tankTurret = nullptr;
}

void Tank::Update(float timeDelta)
{
	bool doesAddNewBreadCrumb = m_breadCrumbTimer->Decrement();
	if (doesAddNewBreadCrumb)
	{
		m_breadCrumbTimer->Reset();
		DebugRender::GetInstance()->CreateDebugPoint(m_transform->GetWorldPosition() + m_transform->GetWorldForward(), .25f, Rgba::GREEN, Rgba::RED, 4.f, LESS_DEPTH_TYPE, m_camera);
	}

	// raycast for targetting =========================================================================================
	Vector3 targetLocation = UpdateTarget(timeDelta);

	// update trajectory renderable =========================================================================================
	UpdateTrajectoryRenderable(targetLocation);
	
	// update position from terrain =========================================================================================
	Vector3 basePosition = m_transform->GetWorldPosition();

	//get height and normal from terrain
	float heightFromTerrain = m_playingState->m_terrain->GetHeightAtPositionXZ(Vector2(basePosition.x, basePosition.z)) + m_baseDimensions.y;	
	Vector3 terrainNormal = m_playingState->m_terrain->GetNormalAtPositionXZ(Vector2(basePosition.x, basePosition.z));

	//calculate new basis from normal (new up)
	Vector3 newRight = CrossProduct(terrainNormal, m_tankBodyTransform->GetWorldForward());
	Vector3 newForward = CrossProduct(newRight, terrainNormal);

	//create a matrix using the new basis that we can extract the rotation from
	Matrix44 newMatrix;
	newMatrix.SetIBasis(Vector4(newRight.GetNormalized(), 0.f));
	newMatrix.SetJBasis(Vector4(terrainNormal.GetNormalized(), 0.f));
	newMatrix.SetKBasis(Vector4(newForward.GetNormalized(), 0.f));
	Vector3 rotation = newMatrix.GetRotation();

	//update the tank transform's position
	m_transform->SetLocalPosition(Vector3(basePosition.x, heightFromTerrain, basePosition.z));
	m_transform->SetLocalRotation(Vector3(0.f, rotation.y, 0.f));

	//update the tank body's rotation
	m_tankBodyTransform->SetLocalRotation(Vector3(rotation.x, 0.f, rotation.z));

	//debug
	DebugRender::GetInstance()->CreateDebugCrosshair2D(Window::GetInstance()->GetCenterOfClientWindow(), Rgba::GREEN, Rgba::GREEN, 0.0f, 1);


	// copy new tank information to tank ui =========================================================================================
	RefreshTankUI();

	//update ui information
	m_tankInformation->Update(timeDelta);
}

void Tank::SetCamera(Camera* camera)
{
	m_camera = camera;

	m_cameraPivotTransform->AddChildTransform(m_camera->m_transform);
}

void Tank::UpdateFromInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();

	// update mouse input =============================================================================
	Vector2 mouseDelta = Vector2::ZERO;		

	mouseDelta = InputSystem::GetInstance()->GetMouse()->GetMouseDelta();				

	// calculate rotation for camera and use same rotation for tank ============================================================================
	m_cameraPivotTransform->AddRotation(Vector3(mouseDelta.y, mouseDelta.x, 0.f) * (timeDelta * 10.f));

	// update movement =============================================================================
	Vector3 positionToAdd = Vector3::ZERO;
	Vector3 positionAtStartOfFrame = positionToAdd;

	if(theInput->IsKeyPressed(theInput->KEYBOARD_W))
	{
		//calculate movement for camera and use same movement for Tank and light
		positionToAdd = m_transform->GetWorldForward() * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_S))
	{	
		positionToAdd = (-1.f * m_transform->GetWorldForward()) * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);		
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_A))
	{
		if(theInput->IsKeyPressed(theInput->KEYBOARD_W) || theInput->IsKeyPressed(theInput->KEYBOARD_S))
		{	
			float multiplyValue = 10.f;

			if (theInput->IsKeyPressed(theInput->KEYBOARD_SHIFT))
			{
				multiplyValue *= 3.f;
			}

			Vector3 rotation = Vector3(0.f, -10.f, 0.f) * timeDelta * multiplyValue;
			m_transform->AddRotation(rotation);
		}
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_D))
	{
		if(theInput->IsKeyPressed(theInput->KEYBOARD_W) || theInput->IsKeyPressed(theInput->KEYBOARD_S))
		{	
			float multiplyValue = 10.f;

			if (theInput->IsKeyPressed(theInput->KEYBOARD_SHIFT))
			{
				multiplyValue *= 3.f;
			}

			Vector3 rotation = Vector3(0.f, 10.f, 0.f) * timeDelta * multiplyValue;
			m_transform->AddRotation(rotation);
		}		
	}

	if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
	{
		Matrix44 turretToWorld = m_turret->m_transform->GetWorldMatrix();
		m_playingState->SpawnBullet(m_turret->m_transform->GetWorldPosition(), turretToWorld.GetRotation());
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_0))
	{
		m_currentHealth--;
	}

	theRenderer = nullptr;
	theInput = nullptr;
}


void Tank::RefreshTankUI()
{
	m_tankInformation->m_tankHealthThisFrame = m_currentHealth;
}

Vector3 Tank::UpdateTarget(float deltaSeconds)
{
	RayCastHit3 raycastResult = RaycastFromCamera(deltaSeconds);

	// update tank turret to look at to result location =========================================================================================
	Matrix44 turretWorld = m_turret->m_transform->GetWorldMatrix();
	Vector3 turretWorldUp = turretWorld.GetUp();

	Matrix44 turretLookAt = turretWorld.LookAt(turretWorld.GetPosition(), raycastResult.position, turretWorldUp);

	Matrix44 lerpLookAt = turretWorld.TurnToward(turretLookAt, 1.f);

	Matrix44 worldToTank = m_transform->GetWorldMatrix().GetInverse();
	worldToTank.Append(lerpLookAt);

	Vector3 rotation = Vector3(worldToTank.GetRotation().x, worldToTank.GetRotation().y, 0.f);
	m_turret->m_transform->SetLocalRotation(rotation); //this is the final turret look at


	// render debug position =========================================================================================
	Vector3 raycastRenderStartPosition = m_transform->GetLocalPosition() + (m_transform->GetWorldUp());

	//turret forward debug line
	//DebugRender::GetInstance()->CreateDebugLine(m_turret->m_transform->GetWorldPosition(), raycastResult.position, Rgba::ORANGE, Rgba::ORANGE, 0.f, 1, m_camera);

	////tank forward debug line
	//DebugRender::GetInstance()->CreateDebugLine(m_transform->GetWorldPosition(), raycastResult.position, Rgba::RED, Rgba::RED, 0.f, 1, m_camera);
	//DebugRender::GetInstance()->CreateDebugCube(raycastResult.position, Vector3(1.f, 1.f, 1.f), Rgba::RED, Rgba::RED, 0.f, 1, m_camera);

	return raycastResult.position;
}

RayCastHit3 Tank::RaycastFromCamera(float deltaSeconds)
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

		float heightAtPosition = m_playingState->m_terrain->GetHeightAtPositionXZ(Vector2(currentPosition.x, currentPosition.z));
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

void Tank::UpdateTrajectoryRenderable(const Vector3& target)
{
	m_renderScene->RemoveRenderable(m_trajectory);
	
	for (int meshIndex = 0; meshIndex < (int)m_trajectory->m_meshes.size(); ++meshIndex)
	{
		delete(m_trajectory->m_meshes[meshIndex]);
		m_trajectory->m_meshes[meshIndex] = nullptr;
	}
	m_trajectory->m_meshes.clear();

	MeshBuilder meshBuilder;

	meshBuilder.CreateLine(m_turret->m_transform->GetWorldPosition(), target, Rgba::RED);
	meshBuilder.CreateCube(target, Vector3::ONE, Rgba::RED);
	m_trajectory->AddMesh(meshBuilder.CreateMesh<VertexPCU>());

	m_renderScene->AddRenderable(m_trajectory);
}