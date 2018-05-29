#include "GameCommon.hpp"
//#include AudioSystem

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
bool g_isDebug = false;
bool g_isQuitting = false;
bool g_isGodMode  = false;
int g_gameState = attract;
bool g_isPausedDisabled = false;
bool g_wasPlayStateJustSet = false;
bool g_didCompleteMap = false;

bool g_isSlowed = false;
bool g_isPlayerShooting = false;

//TODO: Following 3 are depricated.
//int g_numTilesX = 30;
//int g_numTilesY = 30;
float g_numStartingTurrets = 10.f;

float g_playerShootCooldown = .1f;
float g_turretShootCooldown = 1.f;
float g_enemyTankShootCooldown = .5f;
float g_bossTankShootCooldown = .25f;
float g_airStrikeExplosionTime = 3.f;
extern float g_airStrikeModeTimer = 10.f;
float g_enemyTankGoalCooldown = 2.f;
float g_enemyTankMovementSpeed = .5f;
float g_maxTurretToPlayerDistance = 20.f;
float g_raycastStepsPerTile = 100.f;

int g_mapCount = 3;



