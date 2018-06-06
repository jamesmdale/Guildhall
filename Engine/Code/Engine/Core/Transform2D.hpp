#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <vector>

class Transform2D
{
public:
	Transform2D(Transform2D* parent = nullptr);
	Transform2D(const Vector2& newPosition, float newRotation, const Vector2& newScale);

	//local setters
	void SetLocalPosition(Vector2 position);
	void SetLocalRotation(float rotation);
	void SetLocalScale(Vector2 scale);

	//local modifiers
	void AddRotation(float rotation);
	void TranslatePosition(Vector2 position);

	//local getters
	float GetLocalPositionX(){return m_localPosition.x;}
	float GetLocalPositionY(){return m_localPosition.y;}

	float GetLocalRotationAroundZ(){return m_localRotation;}

	float GetLocalScaleX(){return m_localScale.x;}
	float GetLocalScaleY(){return m_localScale.x;}

	Vector2 GetLocalPosition(){return m_localPosition;}
	float GetLocalRotation(){return m_localRotation;}
	Vector2 GetLocalScale(){return m_localScale;}

	//Matrix getters
	Matrix44 GetLocalTranslationMatrix(); //gets identity with point;
	Matrix44 GetLocalRotationMatrixZ(); //gets the rotation matrix using m_rotation.z
	Matrix44 GetLocalScaleMatrix(); //gets the scale matrix	

	//World getters
	Matrix44 GetWorldMatrix(); //gets complete matrix (translation, scale, rotation)

	Vector2 GetWorldPosition();
	Vector2 GetWorldUp();
	Vector2 GetWorldRight();

	//transform hiearchy
	void AddChildTransform2D(Transform2D* child);
	void RemoveChildTransform2D(Transform2D* child);

	bool IsTransform2DDirty(); //gets the current state of the dirty flag.
	void SetTransform2DHiearchyDirty();	

	//helper methods
	void CopyPositionRotationScaleFromTransform2D(const Transform2D& copyFrom);
	void ResetPositionData();

public:
	Matrix44 m_transformMatrix;

private:

	TODO("Switch to quaternions. Currently it is handled as Euler Coords");
	Vector2 m_localPosition; //positiion x, y, z	
	float m_localRotation; //rotation aroundz 
	Vector2 m_localScale = Vector2::ONE; //scale in x, y, z

	bool m_isDirty = false;

	//hiearchy
	Transform2D* m_parentTransform2D;
	std::vector<Transform2D*> m_childrenTransform2Ds;
};

