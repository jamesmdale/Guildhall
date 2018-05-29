#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Math\MathUtils.hpp"

void OrbitCamera::SetTarget(Vector3 newTarget)
{
	m_target = newTarget;
}

void OrbitCamera::SetSphericalCoordinate(float radius, float rotation, float azimuth)
{
	m_radius = radius;
	m_rotation = rotation;
	m_azimuth = azimuth;
}

void OrbitCamera::SetSphericalCoordinate(Vector3 sphericalCoordinate)
{
	m_radius = sphericalCoordinate.x;
	m_rotation =  sphericalCoordinate.y;
	m_azimuth =  sphericalCoordinate.z;
}

Vector3 OrbitCamera::GetSphericalCoordinate()
{
	return SphericalToCartesian(m_radius, m_rotation, m_azimuth);
}
