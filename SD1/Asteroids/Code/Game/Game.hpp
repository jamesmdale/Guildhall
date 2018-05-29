#pragma once
#include "Game\Asteroid.hpp"
#include "Game\Entity.hpp"
#include "Game\Ship.hpp"
#include "Game\Bullet.hpp"
#include "Game\GameCommon.hpp"

class Game
{
public: 

	//TODO come remove all this nonsense
	bool isPaused = false;
	bool isSlowed = false;
	bool isCreateAsteroidPressed = false;
	bool isRemoveAsteroidPressed = false;
	bool isAccelerateKeyPressed = false;
	bool isRotateLeftKeyPressed = false;
	bool isRotateRightKeyPressed = false;
	bool isShootKeyPressed = false;
	bool isResetShipKeyPressed = false;
	bool isDebugEnabled = false;
	Asteroid* asteroids[g_maxAsteroidCount];
	Bullet* bullets[g_maxBulletCount];
	Ship* player;

public:
	Game();
	~Game();
	void Update(float TimeDelta);
	void Render();
	void Initialize();
	void RemoveAsteroid(int asteroidIndex);
	void RemoveBullet(int bulletIndex);
	void SpawnSmallerAsteroids(const Asteroid& asteroidToCopy, float spawnAsteroidSize);
	void AddNewAsteroid();
	void AddNewBullet();
	Asteroid* SpawnRandomLargeAsteroid();
	Ship* SpawnShip();
	Bullet* SpawnBullet();
	void ResetShip();
	float UpdateFlagsFromInput(float timeDelta);
};

extern Game* g_Game;






