#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Renderer\SpriteAnimation.hpp"

class Explosion
{

public:
	Explosion(){};
	~Explosion(){};
	explicit Explosion(Vector2 position, float radius, float duration);
	void Update(float deltaTime);
	void Render();
	//int GetTileIndex(); TODO: Make your life simpler

public:
	Vector2 m_position;
	float m_radius;
	float m_duration;
	
	SpriteAnimation* m_animation;
};
