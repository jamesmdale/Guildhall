#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"


class Ship : public Entity
{
public: 		
	Vector2 m_orientationVector;
	Vector2 m_angularVelocity;
	Vector2 m_velocity;
	bool m_isAccelerating;
	//bool m_isDead;	

public:
	// Construction/Destruction
	~Ship() {}											// destructor: do nothing (for speed)
	Ship() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Initialize();
	void ResetShipToOrigin();
	Vector2 GetShipNose();
};