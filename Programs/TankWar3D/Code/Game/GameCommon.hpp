#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Rgba.hpp"

//following line will go here when any engine side globals are declared and need to be used
//#include "Engine/Core/EngineCommon.hpp"

const Vector2 g_bottomLeftOrtho = Vector2( -1.f, -1.f);
const Vector2 g_topRightOrtho = Vector2(1.f, 1.f);
const Vector2 g_center = Vector2((g_bottomLeftOrtho.x + g_topRightOrtho.x) * .5f, (g_bottomLeftOrtho.y + g_topRightOrtho.y) * .5f);
const Rgba g_backgroundColor = Rgba(1.f, 1.f, 1.f, 1.f);

//spawner info
const Vector3 g_spawnerDimensions = Vector3(5.f, 10.f, 5.f);
const int g_startingNumSpawners = 1;
const float g_spawnerSpawnRateInSeconds = 2.f;
const float g_spawnerMaxSwarmers = 10.f;
const int g_spawnerStartingHealth = 10;

//swarmer info
const int g_swarmerStartingHealth = 1;
const float g_swarmerMoveSpeed = 2.f;
const float g_swarmerRadius = 1.0f;

//game related globals
extern bool g_isDebug;
extern bool g_isQuitting;