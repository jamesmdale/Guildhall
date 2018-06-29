#pragma once
#include "Game\Menu\MenuState.hpp"
#include "Engine\Core\Terrain.hpp"
#include "Engine\Core\Raycast.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Game\TankUI.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Math\Plane.hpp"

//forward declarations
class Spawner;
class Swarmer;
class Bullet;
class Tank;

class PlayingState : public GameState
{
public:
	PlayingState(Camera* camera, Camera* uiCamera) : GameState(camera)
	{
		m_type = PLAYING_MENU_STATE;
		m_uiCamera = uiCamera;

		m_respawnTimer = new Stopwatch(GetMasterClock());
		m_respawnTimer->SetTimer(g_respawnTimer);
	}

	virtual ~PlayingState() override;
	
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;
	virtual void TransitionIn(float secondsTransitioning) override;
	virtual void TransitionOut(float secondsTransitioning) override;
	virtual void ResetState() override;

	void SpawnBullet(const Vector3& startingPosition, const Vector3& startingRotation);
	void RemoveDeadSwarmer(Swarmer* swarmer);
	void RefreshTankUI();
	void RespawnTank();

public:
	Tank* m_playerTank = nullptr;

	TankUI* m_ui = nullptr;

	RenderScene* m_renderScene = nullptr;
	RenderScene2D* m_renderScene2D = nullptr;
	Camera* m_uiCamera = nullptr;
	Terrain* m_terrain = nullptr;

	GameObject* m_waterPlane = nullptr;

	std::vector<Spawner*> m_spawners;
	std::vector<Swarmer*> m_swarmers;
	std::vector<Bullet*> m_bullets;

	Stopwatch* m_respawnTimer = nullptr;

};


