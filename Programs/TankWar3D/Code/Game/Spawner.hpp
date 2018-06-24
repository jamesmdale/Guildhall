#pragma once
#include <vector>
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Game\Menu\PlayingState.hpp"

class Swarmer;

class Spawner : public GameObject
{
public:
	Spawner();
	virtual ~Spawner() override;
	virtual void Update(float deltaSeconds) override;
	void Initialize();

	Swarmer* SpawnSwarmer();
	void RemoveDeadSwarmer(Swarmer* swarmer);

public:
	PlayingState* m_gameState = nullptr;
	Stopwatch* m_spawnTimer;
	std::vector<Swarmer*> m_swarmers;
	int m_health = 0;
};

