#include "Game\Swarmer.hpp"
#include "Game\Spawner.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Tank.hpp"
#include "Engine\Math\MathUtils.hpp"

Swarmer::Swarmer()
{
	m_health = g_swarmerStartingHealth;
}

Swarmer::~Swarmer()
{
	if (m_parentSpawner != nullptr)
	{
		m_parentSpawner->m_numSwarmers--;
		m_parentSpawner = nullptr;
	}

	delete(m_eyeTransform);
	m_eyeTransform = nullptr;

	m_gameState = nullptr;
}

void Swarmer::Update(float deltaSeconds)
{
	// update from terrain =========================================================================================
	Vector3 basePosition = m_transform->GetWorldPosition();	

	//get height and normal from terrain
	float heightFromTerrain = m_gameState->m_terrain->GetHeightAtPositionXZ(Vector2(basePosition.x, basePosition.z)) + 1.f;
	Vector3 terrainNormal = m_gameState->m_terrain->GetNormalAtPositionXZ(Vector2(basePosition.x, basePosition.z));

	//calculate new basis from normal (new up)
	Vector3 newRight = CrossProduct(terrainNormal, m_transform->GetWorldForward());
	Vector3 newForward = CrossProduct(newRight, terrainNormal);

	//create a matrix using the new basis that we can extract the rotation from
	Matrix44 newMatrix;
	newMatrix.SetIBasis(Vector4(newRight.GetNormalized(), 0.f));
	newMatrix.SetJBasis(Vector4(terrainNormal.GetNormalized(), 0.f));
	newMatrix.SetKBasis(Vector4(newForward.GetNormalized(), 0.f));

	//update the tank transform's position
	m_transform->SetLocalPosition(Vector3(basePosition.x, heightFromTerrain, basePosition.z));

	// swarmer look at =========================================================================================
	Vector3 targetPosition = m_gameState->m_playerTank->m_transform->GetWorldPosition();

	Matrix44 swarmerWorld = m_transform->GetWorldMatrix();
	Vector3 swarmerWorldUp = swarmerWorld.GetUp();
	Matrix44 swarmerLookAt = swarmerWorld.LookAt(swarmerWorld.GetPosition(), targetPosition, swarmerWorldUp);
	Vector3 rotation = Matrix44::GetRotationFromMatrix(swarmerLookAt);

	m_transform->SetLocalRotation(Vector3(rotation.x, rotation.y, 0.f));	
	
	Vector3 positionToAdd = m_transform->GetWorldForward() * deltaSeconds * g_swarmerMoveSpeed;
	m_transform->TranslatePosition(positionToAdd);

	// swarmer to player collision =========================================================================================
	Tank* playerTank = m_gameState->m_playerTank;

	AABB3 tankBody = AABB3(playerTank->m_tankBodyTransform->GetWorldPosition(), playerTank->m_baseDimensions.x, playerTank->m_baseDimensions.y, playerTank->m_baseDimensions.z);

	if(tankBody.DoesOverlapWithSphere(m_transform->GetWorldPosition(), g_swarmerRadius))
	{
		m_health--;
		playerTank->m_currentHealth -= 5;
	}

	playerTank = nullptr;
}

void Swarmer::Initialize()
{
	//update transforms
	m_eyeTransform = new Transform();
	
	MeshBuilder meshBuilder;

	//add main body renderable
	Renderable* bodyRenderable = new Renderable();
	meshBuilder.CreateUVSphere(Vector3::ZERO, g_swarmerRadius, 8, 8, Rgba::WHITE);
	bodyRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	bodyRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("lit")));
	bodyRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienSkin.jpg"));
	AddRenderable(bodyRenderable);

	m_transform->AddChildTransform(bodyRenderable->m_transform);

	//add eye renderable
	Renderable* eyeRenderable = new Renderable();
	meshBuilder.CreateUVSphere(Vector3::ZERO, 0.25f, 10, 10, Rgba::WHITE);
	eyeRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	eyeRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("lit")));
	eyeRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienEye.jpg"));

	//displace and rotate eye forward
	m_eyeTransform->SetLocalPosition(Vector3(0.f, 0.5f, 0.75f));
	m_eyeTransform->AddRotation(Vector3(0.f, 90.f, 0.f));

	m_eyeTransform->AddChildTransform(eyeRenderable->m_transform);

	AddRenderable(eyeRenderable);

	m_transform->AddChildTransform(m_eyeTransform);

	//add renderables to scene
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	//cleanup
	eyeRenderable = nullptr;
	bodyRenderable = nullptr;
}