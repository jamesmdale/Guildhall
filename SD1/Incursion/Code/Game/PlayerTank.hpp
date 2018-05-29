#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"
#include "engine\math\AABB2.hpp"
#include "engine\Renderer\Texture.hpp"

class PlayerTank : public Entity
{
public: 		
	Vector2 m_orientationVector;
	Vector2 m_velocity;
	bool m_isAccelerating;
	AABB2 m_tankBounds;
	Texture* m_baseTexture = nullptr;
	Texture* m_turretTexture = nullptr;
	float m_deathTime;

	float m_turretRotationDegrees;
	Vector2 m_turretOrientationVector;
	float m_shootCooldown;
	bool m_isReadyToShoot;
	 
		

public:
	// Construction/Destruction
	~PlayerTank() {}											// destructor: do nothing (for speed)
	PlayerTank() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Render();	
};