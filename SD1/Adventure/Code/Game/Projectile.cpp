#include "Game/Projectile.hpp"

Projectile::Projectile(ProjectileDefinition* projectileDef) : Entity((EntityDefinition*)projectileDef)
{
	m_projectileDef = projectileDef;
	m_health = 1.f; //TODO: set health as 1 for projectiles for now.  Can be data driven later
	m_damage = (float)GetRandomIntInRange(projectileDef->m_damageAmount.min, projectileDef->m_damageAmount.max);

	/*Texture namedAnimsCount = m_spriteAnimSet->GetCurrentTexture();
	namedAnimsCount = m_spriteAnimSet->GetCurrentTexture();*/
}

void Projectile::Update(float timeDelta)
{
	Entity::Update(timeDelta);
}
