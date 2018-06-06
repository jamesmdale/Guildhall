#pragma once
#include "Game\Menu\MenuState.hpp"

class MainMenuState : public MenuState
{
public:
	MainMenuState(Camera* camera) : MenuState(camera)
	{
		m_state = MAIN_MENU_STATE;
	}

	virtual ~MainMenuState() override;

	virtual void Update(float deltaTime) override;
	virtual void PreRender() override;
	//uses parent Render()
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaTime) override;

public:
	//member variables here.
};

