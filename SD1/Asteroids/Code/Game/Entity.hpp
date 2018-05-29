#pragma once
#include "engine\math\vector2.hpp"
#include "engine\math\Disc2.hpp"
#include "engine\core\Rgba.hpp"

extern constexpr int g_MaxVerticesPerShape = 20; //sets the max vertexes possible for an object to 10;
extern constexpr float g_MaxInnerDiscRadiusSize = 0.8f; //used for randomly generating an entities inner collider in local space (also for asteroids jagged edges)

class Entity
{

public:
	// Construction/Destruction
	~Entity() {}											// destructor: do nothing (for speed)
	Entity() {}											// default constructor: do nothing (for speed)
	Entity( const Entity& copyFrom );	
	void DrawShapeWithVertices();
	void DrawExplodingShapeWithVertices();
	void CloneVerticesToExplodeVertices();
	void ExplodeUpdate(float timeDelta);
	explicit Entity(float rotationInDegrees, float rotationSpeed, float m_movementVelocity, float cosmeticRadius, float radius, float centerX, float centerY, float age, Disc2 innerCollider, Disc2 outerCollider, Vector2 movementVector, Vector2 shapeVertices[g_MaxVerticesPerShape], Rgba color, char Type);		// explicit constructor (from x, y)
	void Update(float timeDelta);
	void Render();
	void Initialize();
	void WrapObjectAroundScreen();	
	Vector2 CalculateDebugVector();

public: 	
	float m_rotationDegrees;
	float m_rotationSpeed;
	float m_movementSpeed;
	float m_cosmeticRadius;
	float m_radius;
	float m_centerX;
	float m_centerY;
	float m_age;
	float m_explodeTime;
	bool m_isExploding;
	bool m_isDead;
	Disc2 m_innerCollider;
	Disc2 m_outerCollider;
	Vector2 m_movementVector;
	Vector2 m_shapeVertices[g_MaxVerticesPerShape];
	Vector2 m_shapeVerticesExploding[g_MaxVerticesPerShape * 2]; //has double the amount of vertices
	Rgba m_color;
	char m_Type;
};

