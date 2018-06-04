#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Math\IntVector2.hpp"

//convenience directions
extern IntVector2 MAP_NORTH;
extern IntVector2 MAP_SOUTH;
extern IntVector2 MAP_EAST;
extern IntVector2 MAP_WEST;

extern IntVector2 UI_UP;
extern IntVector2 UI_DOWN;
extern IntVector2 UI_RIGHT;
extern IntVector2 UI_LEFT;


//game related globals
extern bool g_isDebug;
extern bool g_isQuitting;
extern float g_timeSinceProgramStart;

constexpr float g_minWaitTime = 0.f;
constexpr float g_maxWaitTime = 100.f;
constexpr float g_moveCost = 40.f;
constexpr float g_attackCost = 60.f;
constexpr float g_waitCost = 30.f;

//block logic
constexpr float g_blockChanceFront = 0.2f;
constexpr float g_blockChanceSide = 0.05f;
constexpr float g_blockChanceBack = 0.0f;

//critical strike chance
constexpr float g_critChanceFront = 0.05f;
constexpr float g_critChanceSide = 0.15f;
constexpr float g_critChanceBack = 0.25f;

constexpr float g_floatingTextMovementSpeed = 2.f;
constexpr float g_floatingTextDuration = 3.f;


constexpr float ORTHO_MAX = 15.f;
constexpr float ORTHO_MIN = 5.f;
constexpr float LARGE_FLOAT = 9999.f;
constexpr int LARGE_INT = 9999.f;

enum GameState
{
	NONE_STATE,
	LOADING_STATE,
	MAIN_MENU_STATE,
	PLAYING_STATE,
	DEV_CONSOLE_STATE,
	NUM_GAME_STATES
};

