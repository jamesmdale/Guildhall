#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "TileDefinition.hpp"
#include "Map.hpp"

//following line will go here when any engine side globals are declared and need to be used
//#include "Engine/Core/EngineCommon.hpp"

const Vector2 g_bottomLeftOrtho = Vector2( 0.f, 0.f);
const Vector2 g_topRightOrtho = Vector2(30.f, 30.f); //map is 30x30 for now
const Vector2 g_center = Vector2((g_bottomLeftOrtho.x + g_topRightOrtho.x) * .5f, (g_bottomLeftOrtho.y + g_topRightOrtho.y) * .5f);
const Rgba g_backgroundColor = Rgba(0.f, 0.f, 0.f, 1.f);
constexpr float g_defaultCircleSides = 20.f;

//game related globals
extern bool g_isDebug;
extern bool g_isGodMode;
extern bool g_isPausedDisabled;
extern bool g_wasPlayStateJustSet;
extern bool g_isQuitting;
extern int g_gameState;
extern int g_mapCount;
extern bool g_didCompleteMap;


extern bool g_isPlayerShooting;
extern bool g_isSlowed;
//extern int g_numTilesX;
//extern int g_numTilesY;
extern float g_playerShootCooldown;
extern float g_turretShootCooldown;
extern float g_bossTankShootCooldown;
extern float g_airStrikeExplosionTime;
extern float g_airStrikeModeTimer;
extern float g_enemyTankShootCooldown;
extern float g_enemyTankMovementSpeed;
extern float g_enemyTankGoalCooldown;
extern float g_numStartingTurrets; //TODO will change down the line when we have separate levels with different amounts and fixed positions
extern float g_maxTurretToPlayerDistance;
extern float g_raycastStepsPerTile;
const Rgba g_debugInnerColliderColor = Rgba( 0.f, 1.f, 1.f, 1.f ); //inner collider is cyan 
const Rgba g_debugOuterColliderColor = Rgba( 1.f, 0.f, 1.f, 1.f ); //outer collider is magenta  
const Rgba g_debugMovementVectorColor = Rgba( 1.f, 1.f, 0.f, 1.f ); //movement vector2 thruster is yellow 
const float g_turretLaserDistance = 10.f;
const float g_turretVisibilityDistance = 20.f;
const Rgba g_turretLaserColorStart = Rgba( 1.f, 0.f, 0.f, 1.f );
const Rgba g_turretLaserColorEnd = Rgba( 1.f, 0.f, 0.f, 0.1f );;

//gloabl engine systems go here
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;

enum TileDirectionStep
{
	east,
	west,
	north,
	south,
	northeast,
	northwest,
	southeast,
	southwest,
	TILE_DIRECTION_NUM
};

enum GameState
{
	attract = 0,
	play,
	paused,
	respawnpaused,
	victory,	
	NUM_GAME_STATES
};


