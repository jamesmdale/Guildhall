#pragma once
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include "Engine\Renderer\RenderScene.hpp"


class TurnChange : public Effect
{
public:
	TurnChange(){};
	TurnChange(ePlayerType toPlayerType, float totalEffectTime, RenderScene2D* renderScene);
	
	virtual ~TurnChange() override;

	void InitializeTurnRenderable();

	virtual void Update(float deltaSeconds) override;

public:
	ePlayerType m_toPlayerType = NUM_PLAYER_TYPES;
	float m_totalEffectTime;
	RenderScene2D* m_renderScene = nullptr;
	Renderable2D* m_renderable = nullptr;
};

