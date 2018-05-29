#pragma once
#include "Game/Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/RaycastResult.hpp"
#include "Game/Item.hpp"

class Actor : public Entity
{
public:
	Actor(ActorDefinition* actorDef);
	~Actor(){};
	void Update(float timeDelta);
	void Render();
	void UpdatePlayerInput(float timeDelta);
	void UpdateEnemy(float timeDelta);
	void Attack();
	bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos) const;
	RaycastResult Actor::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance ) const;

	//TODO: could use this later to determine which projectile they should fire based on abilities/etc
	//void SetProjectileType(ActorDefinition* newActorDefintion){ m_actorDef = newActorDefintion;}
	//void Render();
public:
	//Actor actions
	bool m_isActorTryingToAttack = false;
	bool m_isActorTryingToInteract = false;
	bool m_isActorTryingToUsePotion = false;
	bool m_isTryingToDropEquipmentItem = false;
	bool m_wasJustHit = false;

	float m_currentHitCooldown = 0.0f;
	const float m_secondsToShowHitDetection= .25f;

	//actor info
	bool m_isReadyToAttack = true;
	bool m_isAttacking = false;
	float m_shootCooldown = 0.f;
	float m_goalCooldown = 0.f;
	Vector2 m_shootDirection = Vector2(0.f, 0.f);
	ActorDefinition* m_actorDef = nullptr;
	const float m_actorGoalCooldownInSeconds = 3.f;

	Stats m_stats;

	std::vector<Item*> m_equippedItems;
};
