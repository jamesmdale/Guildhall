#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Game\Menu\PlayingState.hpp"

class Spawner;
class Swarmer : public GameObject
{
public:
	Swarmer();
	virtual ~Swarmer() override;
	void Initialize();

	virtual void Update(float deltaSeconds) override;

	//swarmer update methods
	void UpdateSwarmerFromTerrain();
	void UpdateSwarmerMovement(float deltaSeconds);
	void SwarmerToTankCollision();

	//flocking methods
	void Flock(float deltaSeconds);
	Vector3 SeparateWithNeighbors(float deltaSeconds);
	Vector3 AlignWithNeighbors(float deltaSeconds);
	Vector3 CenterTowardsNeighbors(float deltaSeconds);
	Vector3 SeekTarget(float deltaSeconds, const Vector3& targetPosition);

	//convenience methods
	inline bool IsAlive(){return m_health > 0 ? true : false;}
	void ApplyForce(const Vector3& force) { m_acceleration += force/m_mass; }

public:
	Spawner* m_parentSpawner = nullptr;
	Transform* m_eyeTransform = nullptr;
	PlayingState* m_gameState = nullptr;
	int m_health = 0;

	//movement
	Vector3 m_velocity;
	Vector3 m_acceleration;
	float  m_mass = 10.f;
};

