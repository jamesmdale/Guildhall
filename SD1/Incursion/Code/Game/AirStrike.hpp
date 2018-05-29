#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Disc2.hpp"

class AirStrike
{

public:
	AirStrike(){};
	~AirStrike(){};
	explicit AirStrike(Vector2 position, float radius, float duration);
	void Update(float deltaTime);
	void Render();
	//int GetTileIndex(); TODO: Make your life simpler

public:
	Vector2 m_position;
	float m_maxRadius;
	float m_duration;
	float m_elapsedSeconds;
	float m_currentRadius;
	bool m_isExploding;
	float m_explosionStartTime;
	bool m_readyForDelete;
	float m_cosmeticRadius;

	Disc2 m_strikeDisc;
};
