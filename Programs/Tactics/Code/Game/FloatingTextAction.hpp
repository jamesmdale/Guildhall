#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Rgba.hpp"
#include <string>
#include "Engine\Core\Transform.hpp"
#include "Game\Actor.hpp"
#include "Game\Encounter.hpp"

class Encounter;
class Actor;

class FloatingTextAction
{
public:
	FloatingTextAction(int index, Actor* targetActor, Encounter* encounter, std::string text, Rgba color);
	~FloatingTextAction();
	void Initialize();

	void Update(float deltaSeconds);
	void Render();
	void CleanupState();
public:
	int m_indexInPendingActionList = -1;
	Actor* m_targetActor;
	Encounter* m_encounter;
	Vector3 m_currentTextPosition;
	std::string m_text;
	Rgba m_color;
	float m_scale = 1.f;
	float m_elapsedTime = 0.0f;	

	Transform m_transform;
};

