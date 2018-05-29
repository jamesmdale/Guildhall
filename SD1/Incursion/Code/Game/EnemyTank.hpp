#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"
#include "engine\math\AABB2.hpp"
#include "engine\Renderer\Texture.hpp"

class EnemyTank : public Entity
{
public: 		
	Vector2 m_orientationVector;
	Vector2 m_velocity;
	bool m_isAccelerating;

	float m_shootCooldown;
	bool m_isReadyToShoot;
	bool m_doesHaveTargetLineOfSight;
	float m_targetDegrees;
	Vector2 m_targetPosition;
	bool m_isChargingPlayer;

	float m_goalCooldown;

public:
	// Construction/Destruction
	~EnemyTank() {}											// destructor: do nothing (for speed)
	EnemyTank() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Render();	
};