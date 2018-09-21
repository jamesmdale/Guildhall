#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
//#include AudioSystem

bool g_isDebug = false;
bool g_isQuitting = false;

//data set in game startup after window has been initialized
float g_tileSize = 1.f;
float g_divideTileSize = 1.f;


//XY
IntVector2 MAP_NORTH = IntVector2(0, 1);
IntVector2 MAP_SOUTH = IntVector2(0, -1);
IntVector2 MAP_EAST = IntVector2(-1, 0);
IntVector2 MAP_WEST = IntVector2(1, 0);

IntVector2 UI_UP = IntVector2(0, 1);
IntVector2 UI_DOWN = IntVector2(0, -1);
IntVector2 UI_RIGHT = IntVector2(-1, 0);
IntVector2 UI_LEFT = IntVector2(1, 0);

