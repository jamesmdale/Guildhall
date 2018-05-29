#pragma once
#include "engine\math\Vector2.hpp"
class Camera
{
public:
	Camera(){};
	Camera(float numTilesHorizontally, float numTilesVertically);
public:
	Vector2 m_worldPos = Vector2(0,0);
	float m_orientationDegrees = 0;
	float m_numTilesViewVertically;
	float m_numTilesViewHorizontally;
};