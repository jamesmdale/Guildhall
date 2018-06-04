#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Game\LightObject.hpp"
#include "Engine\Math\Vector3.hpp"


class Bullet : public GameObject
{
public:
	Bullet(const Vector3& startingPosition, const Vector3& startingRotation);
	~Bullet();

	virtual void Update(float timeDelta) override;
	virtual void PreRender() override;

	void MarkAsDead(){m_isDead = true;}
	bool IsAlive(){return !m_isDead;}
	void UpdateLightDataFromWorldTransform();
	Rgba GetLightColor();

public:
	float m_timeToLive;
	Light* m_light = nullptr;

private:
	bool m_isDead = false;
};

