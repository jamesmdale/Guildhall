#pragma once
#include "Game\Menu\MenuState.hpp"

enum eMainMenuOptions
{
	PLAY,
	EXIT,
	NUM_MAIN_MENU_OPTIONS
};

class MainMenuState : public MenuState
{
public:
	MainMenuState(Camera* camera) : MenuState(camera)
	{
		m_type = MAIN_MENU_STATE;
		m_backGroundTexture = Renderer::GetInstance()->CreateOrGetTexture("default");
	}

	virtual ~MainMenuState() override;

	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

public:
	Texture* m_backGroundTexture;
	eMainMenuOptions m_selectedMenuOption = PLAY;
};

