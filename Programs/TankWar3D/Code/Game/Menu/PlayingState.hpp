#pragma once
#include "Game\Menu\MenuState.hpp"
#include "Game\Tank.hpp"


class PlayingState : public MenuState
{
public:
	PlayingState(Camera* camera) : MenuState(camera)
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

public:
	Tank* m_playerTank = nullptr;
	RenderScene* m_renderScene = nullptr;
};

