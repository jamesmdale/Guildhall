#pragma once
#include "Engine\Core\Widget.hpp"

class TankUI : public Widget
{
public:
	TankUI();
	TankUI(const std::string& name) : Widget(name)
	{
		
	}
	virtual ~TankUI() override;

	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	void RefreshRenderables();

public:
	int m_tankHealthThisFrame = 100;

private:
	int m_tankHealthLastFrame = 100;

};

