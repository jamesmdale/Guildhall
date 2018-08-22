#include "Game\Spawner.hpp"
#include "Game\Swarmer.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Profiler\Profiler.hpp"


Spawner::Spawner()
{
	m_spawnTimer = new Stopwatch(GetMasterClock());
	m_spawnTimer->SetTimer(g_spawnerSpawnRateInSeconds);

	m_health = g_spawnerStartingHealth;
}

Spawner::~Spawner()
{
	m_gameState = nullptr;

	delete(m_spawnTimer);
	m_spawnTimer = nullptr;
}

void Spawner::Update(float deltaSeconds)
{
	PROFILER_PUSH();

	int val = m_numSwarmers;
	if (m_spawnTimer->HasElapsed() && m_numSwarmers < g_spawnerMaxSwarmers)
	{
		SpawnSwarmer();
		m_spawnTimer->Reset();
	}
}

void Spawner::Initialize()
{
	MeshBuilder meshBuilder;

	//add main body renderable
	Renderable* bodyRenderable = new Renderable();
	meshBuilder.CreateCube(Vector3::ZERO, g_spawnerDimensions, Rgba::WHITE);
	bodyRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	bodyRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("lit")));
	bodyRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienShip.jpg"));
	AddRenderable(bodyRenderable);

	m_transform->AddChildTransform(bodyRenderable->m_transform);

	//add renderables to scene
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}
}

void Spawner::SpawnSwarmer()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Swarmer* swarmer = new Swarmer();
	swarmer->m_parentSpawner = this;
	swarmer->m_renderScene = m_gameState->m_renderScene;
	swarmer->m_gameState = m_gameState;
	swarmer->Initialize();

	Vector3 spawnPosition = m_transform->GetWorldPosition();
	
	//float height = m_gameState->m_terrain->GetHeightAtPositionXZ(Vector2(spawnPosition.x, spawnPosition.z));
	swarmer->m_transform->SetLocalPosition(spawnPosition);

	//add swarmer to spawner list
	m_numSwarmers++;

	//add swarmer to spawner list
	m_gameState->m_swarmers.push_back(swarmer);

	theRenderer = nullptr;
}
