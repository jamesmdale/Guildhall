#pragma once
#include "engine/math/vector2.hpp"
#include "engine/math/Disc2.hpp"
#include "engine/Renderer/Texture.hpp"
#include "Engine/math/AABB2.hpp"
#include "game/EntityDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Game/Tags.hpp"


extern constexpr int g_maxVerticesPerShape = 20; //sets the max vertexes possible for an object to 10;
extern constexpr float g_maxInnerDiscRadiusSize = 0.8f; //used for randomly generating an entities inner collider in local space (also for asteroids jagged edges)

class Item;
class Entity
{

public:
	// Construction/Destruction
	~Entity() {}											// destructor: do nothing (for speed)
	Entity() {}											// default constructor: do nothing (for speed)
	explicit Entity(EntityDefinition* entityDef);		// explicit constructor (from x, y)
	virtual void Update(float timeDelta);
	virtual void Render();

public: 	
	float m_movementSpeed = 0.0f;
	Vector2 m_center = Vector2(0,0);
	Vector2 m_movementVector = Vector2(0,0);
	
	float m_age = 0.0f;
	bool m_isAccelerating = false;
	float m_health = 10.f;
	float m_maxHealth = 100.f;
	bool m_isDead = false;

	Tags m_tags;
	std::vector<Item*> m_inventory;

	EntityDefinition* m_entityDef = nullptr;	
	SpriteAnimSet* m_spriteAnimSet = nullptr;
};