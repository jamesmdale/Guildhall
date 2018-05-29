#pragma once
#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"
#include "engine\math\AABB2.hpp"
#include "engine\Renderer\Texture.hpp"

class BossTank : public Entity
{
public: 		
	Vector2 m_orientationVector;
	Vector2 m_velocity;

	float m_shootCooldown;
	bool m_isReadyToShoot;
	bool m_doesHaveTargetLineOfSight;
	float m_targetDegrees;
	Vector2 m_targetPosition;
	int m_attackMode;

	float m_shootCount;
	float m_airStrikeModeTimer;
	float m_goalCooldown;

public:
	// Construction/Destruction
	~BossTank() {}											// destructor: do nothing (for speed)
	BossTank() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Render();	
};

enum AttackMode
{
	normalFire,
	airStrike,
	NUM_FIRING_MODES
};