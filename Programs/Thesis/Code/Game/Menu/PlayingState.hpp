#pragma once
#include "Game\Menu\MenuState.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Time\Stopwatch.hpp"


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

public:
	RenderScene* m_renderScene = nullptr;
	RenderScene2D* m_renderScene2D = nullptr;
	Camera* m_uiCamera = nullptr;

	Stopwatch* m_respawnTimer = nullptr;
};


