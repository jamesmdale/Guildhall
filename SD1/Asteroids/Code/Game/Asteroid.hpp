#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"


class Asteroid : public Entity
{

public:
	// Construction/Destruction
	~Asteroid() {}											// destructor: do nothing (for speed)
	Asteroid() {}											// default constructor: do nothing (for speed)
	Asteroid( const Asteroid& copyFrom );	
	explicit Asteroid(float rotationInDegrees, float rotationSpeed, float m_movementVelocity, float radius, float centerX, float centerY, Vector2 movementVector);		// explicit constructor (from x, y)
	
	void Initialize();
	void GenerateVertices();
};