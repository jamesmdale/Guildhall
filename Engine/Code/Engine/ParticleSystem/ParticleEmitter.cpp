#include "Engine\ParticleSystem\ParticleEmitter.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Game\Game.hpp"


ParticleEmitter::ParticleEmitter()
{
	m_transform = new Transform();
	m_renderable = new Renderable();
}


ParticleEmitter::~ParticleEmitter()
{
	m_renderScene->RemoveRenderable(m_renderable);

	if (m_transform != nullptr)
	{
		delete(m_transform);
		m_transform = nullptr;
	}
	
	if (m_renderable != nullptr)
	{
		delete(m_renderable);
		m_renderable = nullptr;
	}

	if (m_renderScene != nullptr)
	{
		m_renderScene = nullptr;
	}

	if(m_camera != nullptr)
	{
		m_camera = nullptr;
	}
}

void ParticleEmitter::Burst()
{
	int numParticlesToBurst = GetRandomIntInRange(m_burst.min, m_burst.max);
	
	SpawnParticles(numParticlesToBurst);
}

void ParticleEmitter::Update(float deltaSeconds)
{	
	MeshBuilder meshBuilder;
	float timePerParticleSpawn = GetTimePerParticleSpawn();

	if(!m_isTemporary)
	{
		if(timePerParticleSpawn > 0.f)
		{
			m_timeUntilNextSpawn -= deltaSeconds;

			if(m_timeUntilNextSpawn <= 0.f)
			{
				//SpawnParticle();
				m_timeUntilNextSpawn = timePerParticleSpawn;
			}
		}
	}	

	for (int particleIndex = 0; particleIndex < (int)m_particles.size(); particleIndex++)
	{
		//MeshBuilder mb;

		if(m_particles[particleIndex].IsDead())
		{
			m_particles.erase(m_particles.begin() + particleIndex);
			particleIndex--;
			return; 
		}
		
		m_particles[particleIndex].Update(deltaSeconds);
		
		meshBuilder.CreateBillboardQuad3d(m_particles[particleIndex].m_position, m_camera->m_transform->GetWorldUp(), m_camera->m_transform->GetWorldRight(), Vector2(1.f, 1.f), Rgba::WHITE);			
	}	

	m_renderable->AddMesh(meshBuilder.CreateMesh<VertexPCU>());
}

void ParticleEmitter::SpawnParticle()
{
	TODO("Data drive more of this later on.  Too many hardcoded things for now");

	Particle particle = Particle();
	particle.m_totalTimeToLive = GetRandomFloatInRange(m_particleTimeToLive.min, m_particleTimeToLive.max);

	particle.m_remainingTimeToLive = particle.m_totalTimeToLive;
	particle.m_mass = 1.f;
	particle.m_position = m_transform->GetWorldPosition();
	particle.m_velocity = GetRandomVector3InSphere(-500.f, 500.f).GetNormalized() * 2.f;
	particle.m_force = m_particleForce;

	m_particles.push_back(particle);
}

void ParticleEmitter::SpawnParticles(int numParticlesToSpawn)
{
	for(int particleIndex = 0; particleIndex < numParticlesToSpawn; particleIndex++)
	{
		SpawnParticle();
	}
}

bool ParticleEmitter::IsReadyForDelete()
{
	if(m_isTemporary && (int)m_particles.size() == 0)
	{
		return true;
	}

	return false;
}

float ParticleEmitter::GetTimePerParticleSpawn()
{
	if(m_spawnRatePerSecond <= 0)
	{
		return 0;
	}

	return 1.f / m_spawnRatePerSecond;
}
