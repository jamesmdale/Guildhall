#pragma once
#include "Game/Entity.hpp"
#include "Game/ProjectileDefinition.hpp"
#include <vector>

class Projectile : public Entity
{
public:
	Projectile(ProjectileDefinition* projectileDef);
	~Projectile(){};

	void Update(float timeDelta);

	//TODO: maybe a good function add later if we want to have morphing projectiles
	//void SetProjectileType(ProjectileDefinition* newProjectileDefintion){ m_projectileDef = newProjectileDefintion;}

public:
	float m_damage = 0.f;
	ProjectileDefinition* m_projectileDef = nullptr;
	std::vector<std::string> m_attackableFactions;
};
