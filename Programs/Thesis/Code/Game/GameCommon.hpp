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

const float g_tilePercentageOfWindow = 0.025f;

//game related globals
extern bool g_isDebug;
extern bool g_isQuitting;

extern float g_tileSize;
extern float g_divideTileSize;
extern float g_halfTileSize;

//poi globals
extern float g_baseRefillTimePerSecond;
extern int g_maxResourceCarryAmount;

//convenience directions
extern IntVector2 MAP_NORTH;
extern IntVector2 MAP_SOUTH;
extern IntVector2 MAP_EAST;
extern IntVector2 MAP_WEST;

extern IntVector2 UI_UP;
extern IntVector2 UI_DOWN;
extern IntVector2 UI_RIGHT;
extern IntVector2 UI_LEFT;

//tints
extern Rgba ARMORY_TINT;
extern Rgba LUMBER_TINT;
extern Rgba MED_TINT;