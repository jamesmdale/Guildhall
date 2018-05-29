#pragma once
#include "Entity.hpp"

class Turret : public Entity
{

public:
	Vector2 m_orientationVector;
	std::string m_baseTexture;
	std::string m_turretTexture;

	float m_turretRotationDegrees;
	Vector2 m_turretOrientationVector;
	float m_shootCooldown;
	float m_turretLaserDrawDistance;
	bool m_isReadyToShoot;
	bool m_doesHaveTargetLineOfSight;
	float m_targetDegrees;
	
	Vector2 m_targetPosition;
	float m_goalCooldown;

public: 		
	// Construction/Destruction
	~Turret() {}											// destructor: do nothing (for speed)
	Turret() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Render();	
};