#include "GameCommon.hpp"
//#include AudioSystem

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
bool g_isDebug = false;
bool g_isQuitting = false;
bool g_isPlayerAlive = false;
SpriteSheet* g_tileSpriteSheet = nullptr;
std::string g_mapChangeTarget = "default";

float g_aspectRatio = 16.f/9.f;
float g_cameraNumTilesViewVertically = 12;
float g_cameraNumTilesViewHorizontally = g_cameraNumTilesViewVertically * g_aspectRatio;

float g_maxNumTilesViewVertically = 50;
float g_maxNumTilesViewHorizontally = g_maxNumTilesViewVertically * g_aspectRatio;


