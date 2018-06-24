#pragma once
#include "Engine\Core\Widget.hpp"
#include "Engine\Time\Stopwatch.hpp"

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
	int m_numEnemiesThisFrame = 0;
	bool m_isPlayerAlive = true;

private:
	int m_tankHealthLastFrame = 100;
	int m_numEnemiesLastFrame = 0;

};

