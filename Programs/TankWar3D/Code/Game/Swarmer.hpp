#pragma once
#include "Engine\Core\GameObject.hpp"

class Spawner;
class Swarmer : public GameObject
{
public:
	Swarmer();
	virtual ~Swarmer() override;

	virtual void Update(float deltaSeconds) override;

	void Initialize();

public:
	Spawner* m_parentSpawner = nullptr;
	Transform* m_eyeTransform = nullptr;
	int m_health = 0;
};

