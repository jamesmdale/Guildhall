#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"

//following line will go here when any engine side globals are declared and need to be used
//#include "Engine/Core/EngineCommon.hpp"

//entity sizes

//board sizes
const float g_boardHandPercentageOfClientWindow = 0.15f; //hand uses fullwidth
const float g_boardBattlefieldPercentageOfClientWindow = 0.345f; //battlefield uses fullwidth
const float g_boardBattlefieldCenterLineThicknessHeight = 0.01f;

//card sizes
const float g_cardPercentageOfClientWindowHeight = 0.1f;
const float g_cardAspectRatio = 0.722f;

//hero sizes
const Vector2 g_heroPortraitPercentageOfClientWindow = Vector2(0.1f, 0.1f);
const Vector2 g_heroAbilityPercentageOfClientWindow = Vector2(0.075f, 0.075f);
const Vector2 g_heroWeaponPercentageOfClientWindow = Vector2(0.075f, 0.075f);

//other
const Vector2 g_manaPercentageOfClientWindow = Vector2(0.1f, 0.1f);
const Vector2 g_endTurnButtonPercentageOfClientWindow = Vector2(0.1f, 0.1f);


//game related globals
extern bool g_isDebug;
extern bool g_isQuitting;

