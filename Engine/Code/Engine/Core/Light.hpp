#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"
/************************************************************************/
/* Light Data                                                           */
/************************************************************************/
struct Light
{
	//organized for std140

	Vector3 m_lightPosition;
	float m_lightDirectionFactor = 0.f; //0 for point. (0-1] for directed light

	Vector4 m_colorAndIntensity = Vector4(1.f, 1.f, 1.f, 1.f); //intensity is 'w'

	Vector3 m_attenuation = Vector3(1.f, 0.f, 0.f); //x + (y * d) + (z * (d * d))	
	float m_lightInnerAngle = 360.f; //360 to 360 for light in all directions

	Vector3 m_lightForward;
	float m_lightOuterAngle = 360.f; //360 to 360 for light in all directions

									 /*ATTENUATION NOTES:
									 InfiniteLight = (1, 0, 0)
									 Physical = (0, 0, 1)
									 Fudged Physical = (0, 1, 0.75)*/
	
	//shadow casting features
	Matrix44 m_viewProjectionMatrix = Matrix44::IDENTITY;
	
	//padding for shadow casting
	Vector3 m_padding = Vector3::ZERO;
	float m_isShadowCasting = 0.f;


	void Disable() { m_colorAndIntensity.w = 0.0f;};
	void SetToDefault()
	{
		m_lightPosition = Vector3::ZERO;
		m_lightDirectionFactor = 0.f; //0 for point. (0-1] for directed light
		m_colorAndIntensity = Vector4(1.f, 1.f, 1.f, 1.f); //intensity is 'w'
		m_lightInnerAngle = 360.f; //360 to 360 for light in all directions	
		m_attenuation = Vector3(1.f, 0.f, 0.f); //x + (y * d) + (z * (d * d))	
		m_lightOuterAngle = 360.f; //360 to 360 for light in all directions
		m_lightForward = Vector3::FORWARD;
		m_viewProjectionMatrix = Matrix44::IDENTITY;
		m_padding = Vector3::ZERO;
		m_isShadowCasting = 0.f;
	}
};