#pragma once
#include "Engine\Core\GameObject.hpp"

enum AsteroidLevel
{
	ASTEROID_LEVEL_1,
	ASTEROID_LEVEL_2,
	ASTEROID_LEVEL_3,
	NUM_ASTEROID_LEVELS
};

class Asteroid : public GameObject
{
public:

	Asteroid(AsteroidLevel level);
	Asteroid();
	~Asteroid();

	float GetAsteroidRadius();

public:
	AsteroidLevel m_asteroidLevel;
	int m_health;
};

