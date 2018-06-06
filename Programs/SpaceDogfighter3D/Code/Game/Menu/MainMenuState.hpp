#pragma once
#include "Game\Menu\MenuState.hpp"

class MainMenuState : public MenuState
{
public:
	MainMenuState(ForwardRenderingPath* renderingPath, Camera* camera) : MenuState(renderingPath, camera)
	{
		//m_state = MAIN_MENU_STATE;
	}

	virtual ~MainMenuState() override;

	virtual void Update(float deltaTime) override;
	virtual void PreRender() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaTime) override;

public:
	//member variables here.
};

