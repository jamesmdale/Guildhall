#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"
#include <string>

//following line will go here when any engine side globals are declared and need to be used
//#include "Engine/Core/EngineCommon.hpp"

const Vector2 g_bottomLeftOrtho = Vector2( 0.f, 0.f);
const Vector2 g_topRightOrtho = Vector2(1000.f, 1000.f);
const Vector2 g_center = Vector2((g_bottomLeftOrtho.x + g_topRightOrtho.x) * .5f, (g_bottomLeftOrtho.y + g_topRightOrtho.y) * .5f);
const Rgba g_backgroundColor = Rgba(0.f, 0.f, 0.f, 1.f);

//game related globals
extern bool g_isDebug;
extern SpriteSheet* g_tileSpriteSheet;
const Rgba g_debugInnerColliderColor = Rgba( 0.f, 1.f, 1.f, 1.f ); //inner collider is cyan 
const Rgba g_debugOuterColliderColor = Rgba( 1.f, 0.f, 1.f, 1.f ); //outer collider is magenta  
constexpr float g_defaultCircleSides = 20.f;
const float g_actorShootCooldown = 3.f;
const float g_raycastStepsPerTile = 100.f;


//TODO: remove this and make it data driven later
const int g_enemySpawnOdds = 40;

//gloabl engine systems go here
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern bool g_isQuitting;
extern bool g_isPlayerAlive;
extern std::string g_mapChangeTarget;


//camera and view variables
extern float g_aspectRatio;
extern float g_cameraNumTilesViewVertically;
extern float g_cameraNumTilesViewHorizontally;
extern float g_maxNumTilesViewVertically;
extern float g_maxNumTilesViewHorizontally;

//cardinals
const Vector2 g_eastVector2 = Vector2(1,0);
const Vector2 g_westVector2 = Vector2(-1,0);
const Vector2 g_northVector2 = Vector2(0,1);
const Vector2 g_southVector2 = Vector2(0,-1);
const Vector2 g_idleVector2 = Vector2(0,0);

//colors
const Rgba g_green = Rgba(0, 255, 0, 255);
const Rgba g_red = Rgba(255, 0, 0, 255);
const Rgba g_blue = Rgba(0, 0, 255, 255);
const Rgba g_defaultColor = Rgba(1, 1, 1, 1);

//stats
const int g_maxArmorStat = 100;
const int g_maxStrengthStat = 100;
const int g_maxDamageStat = 100;



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
	NUM_TILE_DIRECTION
};

enum GameState
{
	ATTRACT, 
	PLAYING, 
	PAUSED, 
	DEFEAT, 
	VICTORY,
	NONE,
	NUM_GAMESTATES
};
