#include "Engine\Core\Transform.hpp"


Transform::Transform(Transform* parentTransform)
{
	if(parentTransform != nullptr)
	{
		parentTransform->AddChildTransform(this);
		
		if(parentTransform->IsTransformDirty())
		{
			SetTransformHiearchyDirty();
		}
	}
}

Transform::Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	SetLocalPosition(position);
	SetLocalRotation(rotation);
	SetLocalScale(scale);

	SetTransformHiearchyDirty();
}

Transform::~Transform()
{
	m_parentTransform = nullptr;
	
	for (int childIndex = 0; childIndex < (int)m_childrenTransforms.size(); ++childIndex)
	{
		RemoveChildTransform(m_childrenTransforms[childIndex]);
	}
}

void Transform::SetLocalPosition(Vector3 position)
{
	if(position != m_localPosition)
	{
		SetTransformHiearchyDirty();
		m_localPosition = position;
	}
}

void Transform::SetLocalRotation(Vector3 rotation)
{
	if(rotation != m_localRotation)
	{
		SetTransformHiearchyDirty();
		m_localRotation = rotation;
	}	
}

void Transform::SetLocalScale(Vector3 scale)
{
	if(scale != m_localScale)
	{
		SetTransformHiearchyDirty();
		m_localScale = scale;
	}	
}

void Transform::SetRotationFromMatrix(const Matrix44& matrix)
{
	//SetLocalPosition(matrix.GetPosition());
	SetLocalRotation(matrix.GetRotation());
}

void Transform::SetFromMatrix(const Matrix44 & matrix)
{
	SetLocalScale(matrix.GetScale());
	SetLocalRotation(matrix.GetRotation());
	SetLocalPosition(matrix.GetPosition());	
}


void Transform::SetIBasis(const Vector4& iBasis)
{
	m_transformMatrix.SetIBasis(iBasis);
	SetTransformHiearchyDirty();
}

void Transform::SetJBasis(const Vector4& jBasis)
{
	m_transformMatrix.SetJBasis(jBasis);
	SetTransformHiearchyDirty();
}

void Transform::SetKBasis(const Vector4& kBasis)
{
	m_transformMatrix.SetKBasis(kBasis);
	SetTransformHiearchyDirty();
}

void Transform::AddRotation(Vector3 rotation)
{
	if(rotation != Vector3::ZERO)
	{
		m_localRotation += rotation;
		SetTransformHiearchyDirty();
	}
}

void Transform::TranslatePosition(Vector3 position)
{
	if(position != Vector3::ZERO)
	{
		m_localPosition += position;
		SetTransformHiearchyDirty();
	}
}

//gets identity with point;
Matrix44 Transform::GetLocalTranslationMatrix()
{
	return Matrix44::MakeTranslation3D(m_localPosition);
}

//gets the rotation matrix using m_rotation.x
Matrix44 Transform::GetLocalRotationMatrixX() 
{
	return Matrix44::MakeRotationAroundX3D(m_localRotation.x);
}

//gets the rotation matrix using m_rotation.y
Matrix44 Transform::GetLocalRotationMatrixY()
{
	return Matrix44::MakeRotationAroundY3D(m_localRotation.y);
}

//gets the rotation matrix using m_rotation.z
Matrix44 Transform::GetLocalRotationMatrixZ()
{
	return Matrix44::MakeRotationAroundZ3D(m_localRotation.z);
}

//gets the scale matrix
Matrix44 Transform::GetLocalScaleMatrix()
{
	return Matrix44::MakeScale3D(m_localScale.x, m_localScale.y, m_localScale.z);
}

//gets complete matrix
Matrix44 Transform::GetWorldMatrix()
{
	if(m_isDirty == true)
	{
		m_transformMatrix = Matrix44::IDENTITY;

		if(m_parentTransform != nullptr)
		{
			m_transformMatrix.Append(m_parentTransform->GetWorldMatrix());
		}

		Matrix44 tanslationMatrix = GetLocalTranslationMatrix();

		//translate
		m_transformMatrix.Append(GetLocalTranslationMatrix());

		//rotate
		m_transformMatrix.Append(Matrix44::MakeRotationMatrix(m_localRotation));		

		//scale
		m_transformMatrix.Append(GetLocalScaleMatrix());		
	}	

	m_isDirty = false;
	return m_transformMatrix;
}

Vector3 Transform::GetWorldPosition()
{
	if(m_parentTransform != nullptr)
	{
	   return GetWorldMatrix().GetPosition();
	}
	else
	{
		return GetLocalPosition();
	}
}

Vector3 Transform::GetWorldUp()
{
	return GetWorldMatrix().GetUp();
}

Vector3 Transform::GetWorldForward()
{
	return GetWorldMatrix().GetForward();
}

Vector3 Transform::GetWorldRight()
{
	return GetWorldMatrix().GetRight();
}

bool Transform::IsTransformDirty()
{
	return m_isDirty;
}

void Transform::SetTransformHiearchyDirty()
{
	m_isDirty = true;
	
	for (Transform* childTransform : m_childrenTransforms)
	{
		childTransform->SetTransformHiearchyDirty();
	}
}
//
//void Transform::UpdateTransformMatrix()
//{
//	if(m_isDirty == true)
//	{
//		m_transform = Matrix44::IDENTITY;
//
//		if(m_parentTransform != nullptr)
//		{
//			m_transform.Append(m_parentTransform->GetTransformMatrix());
//		}		
//
//		//translate
//		m_transform.Append(GetTranslationMatrix());
//
//		//rotate
//		m_transform.Append(Matrix44::MakeRotationMatrix(m_rotation));		
//
//		//scale
//		m_transform.Append(GetScaleMatrix());
//	}
//
//	m_isDirty = false;
//}

void Transform::AddChildTransform(Transform* childTransform)
{
	childTransform->m_parentTransform = this;
	m_childrenTransforms.push_back(childTransform);
}

void Transform::RemoveChildTransform(Transform * child)
{
	for(int childIndex = 0; childIndex < (int)m_childrenTransforms.size(); childIndex++)
	{
		if(m_childrenTransforms[childIndex] == child)
		{
			m_childrenTransforms[childIndex]->m_parentTransform = nullptr;
			m_childrenTransforms[childIndex] = nullptr;
			m_childrenTransforms.erase(m_childrenTransforms.begin() + childIndex);
		}
	}
}

void Transform::CopyPositionRotationScaleFromTransform(const Transform& copyFrom)
{
	SetLocalPosition(copyFrom.m_localPosition);
	SetLocalRotation(copyFrom.m_localRotation);
	SetLocalScale(copyFrom.m_localScale);
}

void Transform::ResetPositionData()
{
	SetLocalPosition(Vector3::ZERO);
	SetLocalRotation(Vector3::ZERO);
	SetLocalScale(Vector3::ONE);

	m_transformMatrix = Matrix44::IDENTITY;
}

//Vector3 Transform::GetUp()
//{
//	return m_transform.GetUp();
//}
//
//Vector3 Transform::GetForward()
//{
//	return m_transform.GetForward();
//}
//
//Vector3 Transform::GetRight()
//{
//	return m_transform.GetRight();
//}
