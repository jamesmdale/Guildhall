#pragma once
#include "Game\Menu\MenuState.hpp"
#include "Engine\Core\Terrain.hpp"
#include "Engine\Camera\OrbitCamera.hpp"

class Tank;
class PlayingState : public GameState
{
public:
	PlayingState(Camera* camera) : GameState(camera)
	{
		m_type = PLAYING_MENU_STATE;
		m_renderScene = new RenderScene();
	}

	virtual ~PlayingState() override;
	
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

	void UpdateGameCamera(float deltaSeconds);

	Camera* GetCamera();

	void UpdateCameraFromInput(float deltaSeconds);

public:
	Tank* m_playerTank = nullptr;
	RenderScene* m_renderScene = nullptr;
	Terrain* m_terrain = nullptr;
};


