#pragma once
#include "engine\math\vector2.hpp"
#include "engine\math\Disc2.hpp"
#include "engine\core\Rgba.hpp"
#include "engine\Renderer\Texture.hpp"
#include "Engine\math\AABB2.hpp"

extern constexpr int g_MaxVerticesPerShape = 20; //sets the max vertexes possible for an object to 10;
extern constexpr float g_MaxInnerDiscRadiusSize = 0.8f; //used for randomly generating an entities inner collider in local space (also for asteroids jagged edges)

class Entity
{

public:
	// Construction/Destruction
	~Entity() {}											// destructor: do nothing (for speed)
	Entity() {}											// default constructor: do nothing (for speed)
	Entity( const Entity& copyFrom );
	explicit Entity(float rotationInDegrees, float rotationSpeed, float m_movementVelocity, float cosmeticRadius, float radius, Vector2 center, float age, float health, Disc2 innerCollider, Disc2 outerCollider, Vector2 movementVector, Rgba color, char Type);		// explicit constructor (from x, y)
	void Update(float timeDelta);
	void Render();
	void Initialize();
	Vector2 CalculateDebugVector();

public: 	
	float m_rotationDegrees;
	float m_rotationSpeed;
	float m_movementSpeed;
	float m_cosmeticRadius;
	float m_radius;
	Vector2 m_center;
	float m_age;
	float m_health;
	bool m_isDead;
	Disc2 m_innerCollider;
	Disc2 m_outerCollider;
	Vector2 m_movementVector;
	std::string m_texture;
	AABB2 m_textureBounds;
	Rgba m_color;
	char m_Type;
};

enum EntityType
{
	playertank = 0,
	enemytank,
	enemyturret,
	playerbullet,
	enemybullet,
	NUM_ENTITY_TYPES
};