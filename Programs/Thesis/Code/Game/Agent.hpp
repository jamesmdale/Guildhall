#pragma once
#include "Game\Sprites\IsoSpriteAnimSet.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Transform2D.hpp"

//forward declarations
class Planner;
class Map;
class PlayingState;
class Agent;

//typedefs
typedef bool (*ActionCallback)(Agent* agent, const Vector2& goalDestination, int interactEntityId);

//container for data needed to be stored in queue
struct ActionData
{
	ActionCallback m_action = nullptr;
	Vector2 m_finalGoalDestination;
	int m_interactEntityId = -1;
};

class Agent
{
public:
	Agent();

	Agent(Vector2 startingPosition, IsoSpriteAnimSet* animationSet, Map* mapReference);
	~Agent();

	//overriden classes
	void Update(float deltaSeconds);
	void Render();

	void UpdateSpriteRenderDirection();
	//void DetectAgentToTileCollision(Agent, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection);	

	void TakeDamage(int damageAmount);

	//pathing
	bool GetPathToDestination(const Vector2& goalDestination);
	bool GetIsAtPosition(const Vector2& goalDestination);

public:

	int m_id = -1;
	int m_health = 100;

	// bias ----------------------------------------------
	//float m_combatBias = 0.33f;
	//float m_repairBias = 0.33f;
	//float m_healBias = 0.33f;

	// skill ----------------------------------------------
	//float m_combatSkill = 1.f;
	//float m_repairSkill = 1.f;
	//float m_healSkill = 1.f

	// inventory
	int m_arrowCount = 0;
	int m_bandageCount = 0;
	int m_lumberCount = 0;

	// position logic
	Vector2 m_position;
	Vector2 m_forward;
	Vector2 m_intermediateGoalPosition;	//used for temp locations while pathing
	Transform2D m_transform;
	float m_movespeed = 50.f;

	//goal logic	
	std::vector<Vector2> m_currentPath;

	//sprites
	IntVector2 m_spriteDirection;
	IsoSpriteAnimSet* m_animationSet = nullptr;			

	//helper references
	Planner* m_planner = nullptr;
};


// actions ----------------------------------------------
bool MoveAction(Agent* agent, const Vector2& goalDestination, int interactEntityId);		//walk between locations	
bool ShootAction(Agent* agent, const Vector2& goalDestination, int interactEntityId);	    //combat
bool RepairAction(Agent* agent, const Vector2& goalDestination, int interactEntityId);									//repairs
bool HealAction(Agent* agent, const Vector2& goalDestination, int interactEntityId);										//heal
bool GatherAction(Agent* agent, const Vector2& goalDestination, int interactEntityId);		//acquire resource at poiLocation	





