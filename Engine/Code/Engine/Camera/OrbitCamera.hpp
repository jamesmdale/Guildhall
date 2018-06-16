#pragma once
#include "Engine\Camera\Camera.hpp"
class OrbitCamera : public Camera
{

public:
	void SetTarget(Vector3 newTarget);
	void SetSphericalCoordinate(float radius, float rotation, float azimuth);
	void SetSphericalCoordinate(Vector3 sphericalCoordinate);
	Vector3 GetSphericalCoordinate();

public:
	Vector3 m_target;

	float m_radius;      // distance from target
	float m_rotation = 0.f;    // rotation around Z
	float m_azimuth = 0.f;     // rotation around Y
};

