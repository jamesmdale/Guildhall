#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Game\Menu\PlayingState.hpp"

class Spawner;
class Swarmer : public GameObject
{
public:
	Swarmer();
	virtual ~Swarmer() override;

	virtual void Update(float deltaSeconds) override;

	void Initialize();
	inline bool IsAlive(){return m_health > 0 ? true : false;}

public:
	Spawner* m_parentSpawner = nullptr;
	Transform* m_eyeTransform = nullptr;
	PlayingState* m_gameState = nullptr;
	int m_health = 0;
};

