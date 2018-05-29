#pragma once
#include "Entity.hpp"

class Bullet : public Entity
{
public: 		
	// Construction/Destruction
	~Bullet() {}											// destructor: do nothing (for speed)
	Bullet() {}											// default constructor: do nothing (for speed)
	void Update(float timeDelta);
	void Render();	
};