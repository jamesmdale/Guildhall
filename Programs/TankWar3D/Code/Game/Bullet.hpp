#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Game\Menu\PlayingState.hpp"

class Bullet : public GameObject
{
public:
	Bullet(const Vector3& startingPosition, const Vector3& startingRotation);
	virtual ~Bullet() override;

	virtual void Update(float timeDelta) override;

	void MarkAsDead() { m_isDead = true; }
	bool IsAlive() { return !m_isDead; }
	void UpdateLightDataFromWorldTransform();
	Rgba GetLightColor();

public:
	float m_timeToLive;
	Light* m_light = nullptr;
	PlayingState* m_gameState = nullptr;

private:
	bool m_isDead = false;
};
