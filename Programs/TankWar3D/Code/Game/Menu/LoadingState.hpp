#pragma once
#include "Game\Menu\MenuState.hpp"

class LoadingState : public MenuState
{
public:
	LoadingState(Camera* camera) : MenuState(camera)
	{
		m_type = LOADING_MENU_STATE;
		m_backGroundTexture = Renderer::GetInstance()->CreateOrGetTexture("default");
	}

	virtual ~LoadingState() override;

	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

public:
	Texture* m_backGroundTexture;
};

