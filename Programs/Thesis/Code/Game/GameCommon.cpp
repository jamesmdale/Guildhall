#include "Game\GameCommon.hpp"
#include "Game\SimulationData.hpp"
#include "Engine\Window\Window.hpp"

//#include AudioSystem
SimulationData* g_currentSimulationData = nullptr;
int g_currentSimuDefinitionIndex = 0;

bool g_isDebug = false;
bool g_isQuitting = false;
bool g_isIdShown = false;

//data set in game startup after window has been initialized
float g_tileSize = 1.f;
float g_divideTileSize = 1.f;
float g_halfTileSize = 1.f;
bool g_showBlockedTileData = false;

float g_maxCoordinateDistanceSquared = 0.f;

//general globals
int g_maxHealth = 100;
float g_skewForCurrentPlan = 0.05f;

//poi globals
float g_baseResourceRefillTimePerSecond = 0.5f;
int g_maxResourceCarryAmount = 5;

//action performance globals
float g_baseRepairAmountPerPerformance = 5.f;
float g_baseShootDamageAmountPerPerformance = 5.f;
float g_minActionPerformanceRatePerSecond = 0.25f;

float g_minSkillEfficiency = 0.1f;

//bombardment globals
float g_bombardmentExplosionTime = 1.f;
float g_bombardmentExplosionSize = 1.f;
int g_bombardmentDamage = 10.f;

//optimization globals
float g_sortTimerInSeconds = 0.5f;
float g_agentCopyDestinationPositionRadius = 0.5f;

//threat globals
float g_maxThreat = 500.f;

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

