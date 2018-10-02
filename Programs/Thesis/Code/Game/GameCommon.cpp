#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
//#include AudioSystem

bool g_isDebug = false;
bool g_isQuitting = false;

//data set in game startup after window has been initialized
float g_tileSize = 1.f;
float g_divideTileSize = 1.f;
float g_halfTileSize = 1.f;

//poi globals
float g_baseRefillTimePerSecond = 0.5f;
int g_maxResourceCarryAmount = 20;
int g_baseRepairAmountPerSecond = 20;

//bombardment globals
float g_bombardmentRatePerSecond = 1.f;
float g_bombardmentExplosionTime = 1.f;
float g_bombardmentExplosionSize = 1.f;
int g_bombardmentDamage = 10.f;

//threat globals
float g_threatIncreaseRatePerSecond = 1.f;

//XY
IntVector2 MAP_NORTH = IntVector2(0, 1);
IntVector2 MAP_SOUTH = IntVector2(0, -1);
IntVector2 MAP_EAST = IntVector2(1, 0);
IntVector2 MAP_WEST = IntVector2(-1, 0);

IntVector2 UI_UP = IntVector2(0, 1);
IntVector2 UI_DOWN = IntVector2(0, -1);
IntVector2 UI_RIGHT = IntVector2(-1, 0);
IntVector2 UI_LEFT = IntVector2(1, 0);

//TINT
Rgba ARMORY_TINT = Rgba(1.f, 0.f, 0.f, 0.3f);
Rgba LUMBER_TINT = Rgba(0.f, 1.f, 0.f, 0.3f);
Rgba MED_TINT = Rgba(0.f, 0.f, 1.f, 0.3f);

