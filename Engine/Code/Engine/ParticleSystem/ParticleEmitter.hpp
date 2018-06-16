#pragma once
#include "Engine\Math\Vector3.hpp"
#include <vector>
#include "Engine\Core\Transform.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Math\IntRange.hpp"
#include "Engine\Math\FloatRange.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Renderer\RenderScene.hpp"

struct Particle
{

public:

	void Update(float deltaSeconds)
	{
		m_remainingTimeToLive -= deltaSeconds;

		Vector3 acceleration = m_force / m_mass;
		m_velocity += acceleration * deltaSeconds;
		m_position += m_velocity * deltaSeconds;
	}

	inline bool IsDead(){ return m_remainingTimeToLive <= 0;}

	float GetAge()
	{
		return m_totalTimeToLive - m_remainingTimeToLive;
	}

	float GetNormalizedAge()
	{
		return (m_totalTimeToLive - m_remainingTimeToLive) / (m_totalTimeToLive);
	}

	float GetTotalTimeToLive()
	{
		return m_totalTimeToLive;
	}

	float GetRemainingTimeToLive()
	{
		return m_remainingTimeToLive;
	}

public:
	float m_mass;
	Vector3 m_position;
	Vector3 m_velocity; 
	Vector3 m_force;
	float m_totalTimeToLive;
	float m_remainingTimeToLive;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void Burst();
	void Update(float deltaSeconds);

	inline int GetParticleCount() const {return (int)m_particles.size();}
	float GetTimePerParticleSpawn();

	void SpawnParticle();
	void SpawnParticles(int count);

	inline void SetCamera(Camera* camera){ m_camera = camera; }
	inline void SetSpawnRatePerSecond(int particlesPerSecond) {m_spawnRatePerSecond = particlesPerSecond;}	

	 bool IsReadyForDelete();

public:

	Transform* m_transform = nullptr;
	Renderable* m_renderable = nullptr;
	Camera* m_camera = nullptr;
	RenderScene* m_renderScene = nullptr;

	std::vector<Particle> m_particles;

	bool m_isTemporary = false;			//determines whether or not to delete the emitter when no particles are left alive.

	float m_timeUntilNextSpawn = 0.f;
	int m_spawnRatePerSecond;
	Vector3 m_particleForce = Vector3::GRAVITY_FORCE;

	IntRange m_burst = IntRange(0, 0);	//randomly generates a range for how many particles to spawn on initialization
	FloatRange m_particleTimeToLive;	//creates a range for when a particle is spawned a random time to live is generated in range
};

