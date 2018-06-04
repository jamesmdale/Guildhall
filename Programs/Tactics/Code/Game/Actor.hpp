#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Game\ActorDefinition.hpp"
#include "Game\IsoSpriteAnimSet.hpp"
#include "Game\Map.hpp"
#include "Engine\Core\Transform.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"

class Map;
enum ActorFaction
{
	NONE_FACTION,
	RED_ACTOR_FACTION,
	BLUE_ACTOR_FACTION,
	NUM_ACTOR_FACTIONS
};

class Actor
{
public:
	Actor(){};
	Actor::Actor(IntVector2 startingMapPosition, ActorFaction startingFaction, ActorDefinition* definition, Map* mapReference, IntVector2 startingFacingDirection);

	void Update(float deltaSeconds);
	void Render();
	void GetDirectionsFromActorForward(IntVector2 actorForward, IntVector2& actorLeftOut, IntVector2& actorRightOut, IntVector2& actorBackOut);
	void SetSpriteRenderDirection();

	float GetCombatBlockChance(Actor* attackingActor);
	float GetCombatCritChance(Actor* targetingActor);

	void TurnTowardTargetLocation(IntVector2 position);
	IntVector2 GetPositionInXZ();
	void SetPositionXZ(IntVector2 positionXZ);
public:
	Vector3 m_positionXYZ;
	ActorFaction m_faction;
	ActorDefinition* m_definition;
	IsoSpriteAnimSet* m_animationSet = nullptr;

	IntVector2 m_currentForwardInMapDirection;
	IntVector2 m_spriteDirection;

	Transform m_transform;

	Map* m_currentMap = nullptr;

	//originally loaded from definition but can vary or be manipulated by abilities
	int m_turnsSinceMyLastTurn = 0;
	float m_waitTime = 0.0f;
	int m_health;
	float m_blockChance = 0.0f;
};