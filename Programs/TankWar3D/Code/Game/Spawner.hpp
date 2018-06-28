#pragma once
#include <vector>
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Game\Menu\PlayingState.hpp"
#include "Engine\Math\AABB3.hpp"

class Swarmer;

class Spawner : public GameObject
{
public:
	Spawner();
	virtual ~Spawner() override;
	virtual void Update(float deltaSeconds) override;
	void Initialize();

	void SpawnSwarmer();

	inline bool IsAlive(){return m_health > 0 ? true : false;}

public:
	PlayingState* m_gameState = nullptr;
	Stopwatch* m_spawnTimer;
	int m_numSwarmers = 0;
	int m_health = 0;
};

