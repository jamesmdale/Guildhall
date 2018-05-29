#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"

//following line will go here when any engine side globals are declared and need to be used
//#include "Engine/Core/EngineCommon.hpp"

const Vector2 g_bottomLeftOrtho = Vector2( 0.f, 0.f);
const Vector2 g_topRightOrtho = Vector2(1000.f, 1000.f);
const Vector2 g_center = Vector2((g_bottomLeftOrtho.x + g_topRightOrtho.x) * .5f, (g_bottomLeftOrtho.y + g_topRightOrtho.y) * .5f);

//game related globals
const int g_maxAsteroidCount = 1000;
const int g_maxBulletCount = 120;
const float g_maxBulletAge = 2.0f;
constexpr float g_defaultCircleSides = 20.f;

constexpr int g_initialAsteroidCount = 6;
constexpr int g_movementSpeed = 30;
constexpr int g_numShipVertices = 6;
constexpr int g_numBulletVertices = 4;


//explosion variables
constexpr int g_explosionSpeedAsteroid = 5;
constexpr int g_explosionSpeedBullet = 5;
constexpr int g_explosionSpeedShip = 10;
const float g_explosionTimeAsteroid = 1.f;
const float g_explosionTimeBullet = 0.25f;
const float g_explosionTimeShip = 1.f;
constexpr int g_explosionFadeRate = 5;

constexpr float g_smallAsteroidSize = static_cast<float>((1.0f/40.0f) * 1000.0f); //25
constexpr float g_mediumAsteroidSize = static_cast<float>((1.0f/20.0f) * 1000.0f); //50 
constexpr float g_largeAsteroidSize = static_cast<float>((1.0f/10.0f) * 1000.0f);  // 100
const Rgba g_backgroundColor = Rgba( 0.f, 0.f, 0.f, 1.f ); //background is black
const Rgba g_shipColor = Rgba( 0.f, 0.f, 1.f, 1.f ); //ship is blue
const Rgba g_debugInnerColliderColor = Rgba( 0.f, 1.f, 1.f, 1.f ); //inner collider is cyan 
const Rgba g_debugOuterColliderColor = Rgba( 1.f, 0.f, 1.f, 1.f ); //outer collider is magenta  
const Rgba g_debugMovementVectorColor = Rgba( 1.f, 1.f, 0.f, 1.f ); //movement vector2 thruster is yellow 

extern bool g_isDebug;

//gloabl engine systems go here
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;


