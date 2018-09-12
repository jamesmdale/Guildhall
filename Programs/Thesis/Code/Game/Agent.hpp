#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Transform2D.hpp"
#include "Game\Sprites\IsoSpriteAnimSet.hpp"

class Map;
class PlayingState;
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

public:
	Vector2 m_position;
	Vector2 m_forward;
	Vector2 m_goalLocation;
	float m_movespeed = 5.f;

	IntVector2 m_spriteDirection;
	IsoSpriteAnimSet* m_animationSet = nullptr;		

	Transform2D m_transform;

	//helper references
	Map* m_currentMap = nullptr;

};


