#pragma once
#include "Entity.hpp"
#include "engine\math\vector2.hpp"


class Bullet : public Entity
{
public:
	
	~Bullet() {}											// destructor: do nothing (for speed)
	Bullet() {}											// default constructor: do nothing (for speed)	
	void Initialize();
};
