#include "Engine\Math\Trajectory.hpp"
#include <math.h>
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"

Vector2 Trajectory::Evaluate(float gravity, Vector2 launchVelocity, float time)
{
	float launchSpeed = launchVelocity.GetLength();
	float angleDegrees = launchVelocity.GetOrientationDegrees();

	return Evaluate(gravity, launchSpeed, angleDegrees, time);
}

Vector2 Trajectory::Evaluate(float gravity, float launchSpeed, float launchAngle, float time)
{
	//f(t) = (v * cos(theta) * t , (-1/2) * g * (t^2) + v * sin(theta) * t)

	//check gravity sign
	float x = launchSpeed * CosDegrees(launchAngle) * time;
	float y = (-0.5f * gravity * (time * time)) + (launchSpeed * SinDegrees(launchAngle) * time);

	return Vector2(x, y); 
	//rotation 
	//Cartesian to polar of (returnVector) = rotation
	//launch angle is azimuth
	//
	//newPositionInCorrectDirection = SphericalToCarteisan(returnVector.length, rotation, azimuth)
}

float Trajectory::GetMinimumLaunchSpeed(float gravity, float distance)
{
	float minLaunchSpeed = sqrtf(distance * gravity); //could have launch angles as a parameter and divide by // (distance * gravity)/(SinDegrees(2 * launchAngles)
	
	return minLaunchSpeed;
}

bool Trajectory::GetLaunchAngles(Vector2* outAngles, float gravity, float launchSpeed, float distance, float heightDifference)
{
	/*float a = (-1.f * gravity * (distance * distance)) / (2.f * launchSpeed * launchSpeed);
	float b = -1.f * distance;
	float c = heightDifference + a;
*/
	/*float a = launchSpeed * launchSpeed;
	float b = -1.f * distance;
	float c = heightDifference + a;*/

	/*bool isQuadraticValid = SolveQuadratic(&outValues, a, b, c);

	if(isQuadraticValid = false)
	{
		return false;
	}*/

	float launchSpeedSquared = launchSpeed * launchSpeed;
	float distanceSquared = distance * distance;
	
	float determinent = (launchSpeedSquared * launchSpeedSquared) - (gravity * ((gravity * distanceSquared) + (2 * heightDifference * launchSpeedSquared)));

	float thetas = sqrtf((determinent));
	float value1Tan = (launchSpeedSquared + thetas)/(gravity * distance);
	float value2Tan = (launchSpeedSquared - thetas)/(gravity * distance);
	outAngles->x = AtanfAsDegrees(value1Tan, 1.f);
	outAngles->y = AtanfAsDegrees(value2Tan, 1.f);

	if(outAngles->x > outAngles->y)
	{
		float tempAngle = outAngles->y;
		outAngles->y = outAngles->x;
		outAngles->x = tempAngle;
	}

	return true;
}

float Trajectory::GetMaxHeight(float gravity, float launchSpeed, float distance)
{
	float val = (launchSpeed * launchSpeed) / (gravity * distance);
	float radians = atan2f(val, 1.f);

	float tanTheta = tan(radians);

	float maxHeight = 0.5f * (distance * distance);	

	return maxHeight;
}

Vector2 Trajectory::GetLaunchVelocity(float gravity, float apexHeight, float distance, float heightDifference)
{
	//float Vy = gravity * sqrt((2 * apexHeight) / gravity);
	float Vy = sqrt(2 * apexHeight * (-1.f) * gravity);

	float a = 0.5f * gravity;
	float b = Vy;
	float c = -1.f * heightDifference;

	Vector2 outTimeValues;
	bool isQuadraticValid = SolveQuadratic(&outTimeValues, a, b, c);
	GUARANTEE_OR_DIE(isQuadraticValid, "QUADRATIC VALUE IN GETLAUNCHVELOCITY() IS INVALID");

	float Vx = distance / outTimeValues.y;

	return Vector2(Vx, Vy);
}
