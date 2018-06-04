#pragma once
#include <vector>
#include "Game\Actor.hpp"
#include "Game\Map.hpp"
#include "Game\Encounter.hpp"
class Actor;
class Encounter;

class ShootingAction
{
public:
	ShootingAction(Vector3 target, Actor* actor, Map* currentMap, Encounter* currentEncounter);
	void Initialize();
	~ShootingAction();

	void Update(float deltaSeconds);
	void Render();
	void CleanupState();

public:
	Vector3 m_target;
	float m_elapsedTime = 0.0f;
	Vector3 m_projectilePositionXYZ;

	//Trajectory Info
	float m_launchAngle = 0.0f;
	float m_gravity = 9.8f;
	float m_distanceToTarget = 0.0f;
	float m_launchSpeed = 0.0f;
	float m_heightDifference = 0.0f;

	Actor* m_actor;
	Map* m_map;
	Encounter* m_currentEncounter;
};

