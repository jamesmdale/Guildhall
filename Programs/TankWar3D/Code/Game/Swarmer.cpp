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

// udpate methods =========================================================================================

void Swarmer::Update(float deltaSeconds)
{
	//change orientation and height from terrain
	UpdateSwarmerFromTerrain();

	//update movement from swarming and orientation to player
	UpdateSwarmerMovement(deltaSeconds);

	//calculate whether swarmer is colliding with tank
	SwarmerToTankCollision();

	//reset acceleration for the frame
	m_acceleration = Vector3::ZERO;
}

void Swarmer::UpdateSwarmerFromTerrain()
{
	// update from terrain =========================================================================================
	Vector3 basePosition = m_transform->GetWorldPosition();

	//get height and normal from terrain
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
	m_transform->SetLocalPosition(Vector3(basePosition.x, 0.0f, basePosition.z));
}

void Swarmer::UpdateSwarmerMovement(float deltaSeconds)
{
	Vector3 position = m_transform->GetWorldPosition();
	Vector2 basePositionXZ = Vector2(position.x, position.z);
	Flock(deltaSeconds);

	//final velocity calculation =========================================================================================
	m_velocity += m_acceleration;	

	//clamp veloicty
	m_velocity = ClampVector3Uniform(m_velocity, -1.f * Vector3(0.05f, 0.05f, 0.05f), Vector3(0.05f, 0.05f, 0.05f));

	//make sure final height is not greater than terrain height
	float heightFromTerrain = m_gameState->m_terrain->GetHeightAtPositionXZ(basePositionXZ) + 1.f;
	 
	m_transform->TranslatePosition(m_velocity);
	m_transform->SetLocalPositionY(heightFromTerrain);
}

void Swarmer::SwarmerToTankCollision()
{
	// swarmer to player collision =========================================================================================
	Tank* playerTank = m_gameState->m_playerTank;

	AABB3 tankBody = AABB3(playerTank->m_tankBodyTransform->GetWorldPosition(), playerTank->m_baseDimensions.x, playerTank->m_baseDimensions.y, playerTank->m_baseDimensions.z);

	if (tankBody.DoesOverlapWithSphere(m_transform->GetWorldPosition(), g_swarmerRadius))
	{
		m_health--;
		playerTank->m_heatlth -= 20;
	}

	playerTank = nullptr;
}

void Swarmer::Flock(float deltaSeconds)
{
	Vector3 separateSteer = SeparateWithNeighbors(deltaSeconds);
	Vector3 alignSteer = AlignWithNeighbors(deltaSeconds);
	Vector3 centerSteer = CenterTowardsNeighbors(deltaSeconds);
	Vector3 targetSteer = SeekTarget(deltaSeconds, m_gameState->m_playerTank->m_transform->GetWorldPosition());

	separateSteer *= 1.5f;
//	alignSteer *= 1.0f;
//	centerSteer *= 1.0f;
	targetSteer *= 1.0f;

	ApplyForce(separateSteer);
	ApplyForce(alignSteer);
	ApplyForce(centerSteer);
	ApplyForce(targetSteer);
}

Vector3 Swarmer::SeparateWithNeighbors(float deltaSeconds)
{
	Vector3 currentPosition = m_transform->GetWorldPosition();
	Vector2 basePositionXZ = Vector2(currentPosition.x, currentPosition.z);

	Vector3 displacementSum = Vector3::ZERO;
	int numCloseSwarmers = 0;
	Vector3 steerDir = Vector3::ZERO;

	// apply separation forces =========================================================================================
	for (int swarmerIndex = 0; swarmerIndex < (int)m_gameState->m_swarmers.size(); swarmerIndex++)
	{
		//if we are checking against ourselves, go to the next iteration
		if (m_gameState->m_swarmers[swarmerIndex] == this)
			continue;

		Swarmer* swarmer = m_gameState->m_swarmers[swarmerIndex];
		Vector3 swarmerPosition = swarmer->m_transform->GetWorldPosition();
		float distanceToSwarmerSquared = GetDistanceSquared(currentPosition, swarmerPosition);

		if (distanceToSwarmerSquared < g_swarmerMinDistanceToOther * g_swarmerMinDistanceToOther)
		{
			Vector3 displacement = currentPosition - swarmerPosition;
			displacement.Normalize();
			displacementSum += displacement;
			numCloseSwarmers++;
		}

		swarmer = nullptr;
	}

	if (numCloseSwarmers > 0)
	{
		displacementSum = displacementSum / numCloseSwarmers;
		displacementSum *= deltaSeconds * g_swarmerMoveSpeed;

		steerDir = displacementSum - m_velocity;
		steerDir *= deltaSeconds * g_swarmerMoveSpeed;
	}	

	return steerDir;
}

Vector3 Swarmer::AlignWithNeighbors(float deltaSeconds)
{
	Vector3 currentPosition = m_transform->GetWorldPosition();
	Vector3 velocitySum = Vector3::ZERO;

	int numCloseSwarmers = 0;
	Vector3 steerDir;

	// apply separation forces =========================================================================================
	for (int swarmerIndex = 0; swarmerIndex < (int)m_gameState->m_swarmers.size(); swarmerIndex++)
	{
		//if we are checking against ourselves, go to the next iteration
		if (m_gameState->m_swarmers[swarmerIndex] == this)
			continue;

		Swarmer* swarmer = m_gameState->m_swarmers[swarmerIndex];
		Vector3 swarmerPosition = swarmer->m_transform->GetWorldPosition();
		float distanceToSwarmerSquared = GetDistanceSquared(currentPosition, swarmerPosition);
		
		if (distanceToSwarmerSquared < g_swarmerMinDistanceToOther * g_swarmerMinDistanceToOther)
		{
			velocitySum += m_gameState->m_swarmers[swarmerIndex]->m_velocity;
			numCloseSwarmers++;
		}
	}

	if (numCloseSwarmers > 0)
	{
		velocitySum = velocitySum / (float)m_gameState->m_swarmers.size();
		velocitySum.Normalize();
		velocitySum *= deltaSeconds * g_swarmerMoveSpeed;

		steerDir *= deltaSeconds * g_swarmerMoveSpeed;
		steerDir = velocitySum - m_velocity;
	}	

	return steerDir;
}

Vector3 Swarmer::CenterTowardsNeighbors(float deltaSeconds)
{
	Vector3 currentPosition = m_transform->GetWorldPosition();
	Vector3 locationSum = Vector3::ZERO;

	int numCloseSwarmers = 0;
	Vector3 steerDir;

	// apply separation forces =========================================================================================
	for (int swarmerIndex = 0; swarmerIndex < (int)m_gameState->m_swarmers.size(); swarmerIndex++)
	{
		//if we are checking against ourselves, go to the next iteration
		if (m_gameState->m_swarmers[swarmerIndex] == this)
			continue;

		Swarmer* swarmer = m_gameState->m_swarmers[swarmerIndex];
		Vector3 swarmerPosition = swarmer->m_transform->GetWorldPosition();
		float distanceToSwarmerSquared = GetDistanceSquared(currentPosition, swarmerPosition);

		if (distanceToSwarmerSquared < g_swarmerMinDistanceToOther * g_swarmerMinDistanceToOther)
		{
			locationSum += m_gameState->m_swarmers[swarmerIndex]->m_transform->GetWorldPosition();
			numCloseSwarmers++;
		}
	}

	if (numCloseSwarmers > 0)
	{
		locationSum = locationSum / (float)m_gameState->m_swarmers.size();
		return SeekTarget(deltaSeconds, locationSum);
	}

	return steerDir;
}

Vector3 Swarmer::SeekTarget(float deltaSeconds, const Vector3& targetPosition)
{
	Vector3 steerDir = Vector3::ZERO;
	Vector3 desiredDirection = targetPosition - m_transform->GetWorldPosition();
	desiredDirection.Normalize();

	steerDir = desiredDirection - m_velocity;
	steerDir *= deltaSeconds * g_swarmerMoveSpeed;

	return steerDir;
}


// initialization method =========================================================================================

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