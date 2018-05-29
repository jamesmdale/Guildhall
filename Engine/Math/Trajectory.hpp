#pragma once
#include "Engine\Math\Vector2.hpp"


/*
**********************NOTES*************************


f(t) = (v * cos(theta) * t , (-1/2) * g * (t^2) + v * sin(theta) * t)

v = launch speed
g = gravity (generally 9.8)
theta = launch angle
t = time

*/


class Trajectory
{

public:
	static Vector2 Evaluate(float gravity, Vector2 launchVelocity, float time);
	static Vector2 Evaluate(float gravity, float launchSpeed, float launchAngle, float time);


	//return the minimum launch speed (muzzle speed) needed to reach a target distance away.  This will assume an optimal firing angle of 45°
	static float GetMinimumLaunchSpeed(float gravity, float distance);

	//Given a fixed launch speed, determine the angle needed to hit a target distance away, at a given height. 
	//This can be at most two angles.  If only one angle is found, you should repeat for both values of out.
	static bool GetLaunchAngles(Vector2* outAngles,
		float gravity,
		float launchSpeed,
		float distance,			//desired horizontal displacement
		float heightDifference = 0.0f);	//desired vertical displacement

	//Given a launch speed and a distance to reach, display max possible height we could reach by adjusting the angle
	static float GetMaxHeight(float gravity, float launchSpeed, float distance);


	//Given a target apex height and a target (distance and height), determine launch velocity
	//(Assume apex height is positive and greater than target height)
	static Vector2 GetLaunchVelocity(float gravity,
		float apexHeight,  //must be greater than height
		float distance,    //target distance
		float heightDifference);     //target height

public:
	//breaking the m_ rules here
	/*float gravity;
	Vector2 launchVelocity;
	float time;
	float launchSpeed;
	float launchAngle;*/
};

