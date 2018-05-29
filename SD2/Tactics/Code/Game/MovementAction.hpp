#pragma once
#include <vector>
#include "Game\Actor.hpp"
#include "Game\Map.hpp"
#include "Game\Encounter.hpp"

class Actor;
class Encounter;

class MovementAction
{
public:
	MovementAction(std::vector<int> orderedPath, Actor* actor, Map* currentMap, Encounter* currentEncounter);
	void Initialize();
	~MovementAction();

	void Update(float deltaSeconds);
	void CleanupState();

public:
	int m_currentTargetIndex = -1;
	float m_totalFractionOfMovementToCurrentIndex = 1.0f;

	//passed data
	std::vector<int> m_orderedTileIndicesForShortestPath;
	Vector3 m_startPositionForTileMovement;
	Actor* m_actor;
	Map* m_map;
	Encounter* m_currentEncounter;
};

