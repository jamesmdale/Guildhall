#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Game\LightObject.hpp"
#include <vector>
#include "Engine\Renderer\RenderScene.hpp"
#include "Engine\Renderer\ForwardRenderingPath.hpp"
#include "Game\Ship.hpp"
#include "Game\Asteroid.hpp"
#include "Game\Bullet.hpp"
#include "Engine\ParticleSystem\ParticleEmitter.hpp"

class Game
{
public:  
	Camera* m_gameCamera = nullptr;

	ForwardRenderingPath* m_forwardRenderingPath = nullptr;
	RenderScene* m_renderScene = nullptr;

	Ship* m_playerShip = nullptr;
	std::vector<Asteroid*> m_asteroids;
	std::vector<Bullet*> m_bullets;
	std::vector<ParticleEmitter*> m_collisionEmitters;

	bool m_isPaused = false;

public:
	Game();
	~Game();
	void Update(float TimeDelta);
	void PreRender();
	void Render();
	void PostRender();
	void Initialize();
	float UpdateInput(float timeDelta);

	void SpawnBullet(const Vector3& startingPosition, const Vector3& startingRotation);
	void SpawnAsteroid(const Vector3& startingPosition, AsteroidLevel level);
	void SpawnCollisionEmitter(const Vector3& startingPosition, const Vector3& emissionDirection);
};

extern Game* g_theGame;
extern Clock* g_gameClock;






