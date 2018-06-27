#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include <vector>
#include "Engine\Math\Vector4.hpp"

class Transform
{
public:
	Transform(Transform* parent = nullptr);
	Transform(const Vector3& newPosition, const Vector3& newRotation, const Vector3& newScale);

	~Transform();

	//local setters
	void SetLocalPosition(Vector3 position);
	void SetLocalPositionX(float x){m_localPosition.x = x;}
	void SetLocalPositionY(float y){m_localPosition.y = y;}
	void SetLocalPositionZ(float z){m_localPosition.z = z;}

	void SetLocalRotation(Vector3 rotation);
	void SetLocalScale(Vector3 scale);

	//matrix setters
	void SetRotationFromMatrix(const Matrix44& matrix);
	void SetFromMatrix(const Matrix44& matrix);
	void SetIBasis(const Vector4& iBasis);
	void SetJBasis(const Vector4& jBasis);
	void SetKBasis(const Vector4& kBasis);

	//local modifiers
	void AddRotation(Vector3 rotation);
	void TranslatePosition(Vector3 position);

	//local getters
	float GetLocalPositionX(){return m_localPosition.x;}
	float GetLocalPositionY(){return m_localPosition.y;}
	float GetLocalPositionZ(){return m_localPosition.z;}

	float GetLocalRotationAroundX(){return m_localRotation.x;}
	float GetLocalRotationAroundY(){return m_localRotation.y;}
	float GetLocalRotationAroundZ(){return m_localRotation.z;}

	float GetLocalScaleX(){return m_localScale.x;}
	float GetLocalScaleY(){return m_localScale.x;}
	float GetLocalScaleZ(){return m_localScale.x;}

	Vector3 GetLocalPosition(){return m_localPosition;}
	Vector3 GetLocalRotation(){return m_localRotation;}
	Vector3 GetLocalScale(){return m_localScale;}

	//Matrix getters
	Matrix44 GetLocalTranslationMatrix(); //gets identity with point;

	Matrix44 GetLocalRotationMatrixX(); //gets the rotation matrix using m_rotation.x
	Matrix44 GetLocalRotationMatrixY(); //gets the rotation matrix using m_rotation.y
	Matrix44 GetLocalRotationMatrixZ(); //gets the rotation matrix using m_rotation.z
	
	Matrix44 GetLocalScaleMatrix(); //gets the scale matrix	

	//World getters
	Matrix44 GetWorldMatrix(); //gets complete matrix (translation, scale, rotation)

	Vector3 GetWorldPosition();
	Vector3 GetWorldUp();
	Vector3 GetWorldForward();
	Vector3 GetWorldRight();

	//transform hiearchy
	void AddChildTransform(Transform* child);
	void RemoveChildTransform(Transform* child);

	bool IsTransformDirty(); //gets the current state of the dirty flag.
	void SetTransformHiearchyDirty();	

	//helper methods
	void CopyPositionRotationScaleFromTransform(const Transform& copyFrom);
	void ResetPositionData();

public:
	Matrix44 m_transformMatrix;

private:
	TODO("Switch to quaternions. Currently it is handled as Euler Coords");
	Vector3 m_localPosition; //positiion x, y, z	
	Vector3 m_localRotation; //rotation around x, y, z 
	Vector3 m_localScale = Vector3::ONE; //scale in x, y, z
	
	bool m_isDirty = false;

	//hiearchy
	Transform* m_parentTransform;
	std::vector<Transform*> m_childrenTransforms;
};

