#include "Game\Spawner.hpp"
#include "Game\Swarmer.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"


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

	for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); ++swarmerIndex)
	{
		m_swarmers[swarmerIndex] = nullptr;
	}
	m_swarmers.clear();
}

void Spawner::Update(float deltaSeconds)
{
	if (m_spawnTimer->HasElapsed() && (int)m_swarmers.size() < g_spawnerMaxSwarmers)
	{
		Swarmer* swarmer = SpawnSwarmer();

		swarmer = nullptr;
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
	bodyRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("default")));
	bodyRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienShip.jpg"));
	AddRenderable(bodyRenderable);

	m_transform->AddChildTransform(bodyRenderable->m_transform);

	//add renderables to scene
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}
}

Swarmer* Spawner::SpawnSwarmer()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Swarmer* swarmer = new Swarmer();
	swarmer->m_parentSpawner = this;
	swarmer->m_renderScene = m_gameState->m_renderScene;
	swarmer->Initialize();
	//swarmer->m_transform->SetLocalPosition(m_transform->GetWorldPosition());
	
	Vector3 worldPosition = swarmer->m_transform->GetWorldPosition();
	float height = m_gameState->m_terrain->GetHeightAtPositionXZ(Vector2(worldPosition.x, worldPosition.y));
	swarmer->m_transform->SetLocalPosition(Vector3(worldPosition.x, height + 0.5f, worldPosition.y));

	//add swarmer to spawner list
	m_swarmers.push_back(swarmer);

	//add swarmer to spawner list
	m_gameState->m_swarmers.push_back(swarmer);

	//cleanup
	return swarmer;
}

void Spawner::RemoveDeadSwarmer(Swarmer* swarmer)
{
	//remove swarmer from our list. DOES NOT DELETE (just removes and resizes)
	for (int swarmerIndex = 0; swarmerIndex < (int)m_swarmers.size(); ++swarmerIndex)
	{
		if (m_swarmers[swarmerIndex] == swarmer)
		{
			m_swarmers[swarmerIndex] = nullptr;
			m_swarmers.erase(m_swarmers.begin() + swarmerIndex);
		}
	}
}
