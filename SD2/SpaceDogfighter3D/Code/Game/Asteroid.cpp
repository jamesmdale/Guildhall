#include "Game\Asteroid.hpp"

Asteroid::Asteroid(AsteroidLevel level)
{
	m_asteroidLevel = level;
	switch(level)
	{
		case ASTEROID_LEVEL_1:
		{
			m_health = 10;
			m_transform->SetLocalScale(m_transform->GetLocalScale() * 20.f);
			break;
		}
		case ASTEROID_LEVEL_2:
		{
			m_health = 5;
			m_transform->SetLocalScale(m_transform->GetLocalScale() * 10.f);
			break;
		}
		case ASTEROID_LEVEL_3:
		{
			m_health = 3;
			m_transform->SetLocalScale(m_transform->GetLocalScale() * 5.f);
			break;
		}
	}
}


Asteroid::Asteroid()
{
}

Asteroid::~Asteroid()
{
}

float Asteroid::GetAsteroidRadius()
{
	switch(m_asteroidLevel)
	{
		case ASTEROID_LEVEL_1:
		{
			return 20.f;
			break;
		}
		case ASTEROID_LEVEL_2:
		{
			return 10.f;
			break;
		}
		case ASTEROID_LEVEL_3:
		{
			return 5.f;
			break;
		}
	}
}
