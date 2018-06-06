#include "Engine\Core\Transform2D.hpp"


Transform2D::Transform2D(Transform2D* parentTransform2D)
{
	if(parentTransform2D != nullptr)
	{
		parentTransform2D->AddChildTransform2D(this);

		if(parentTransform2D->IsTransform2DDirty())
		{
			SetTransform2DHiearchyDirty();
		}
	}
}

Transform2D::Transform2D(const Vector2& position, float rotation, const Vector2& scale)
{
	SetLocalPosition(position);
	SetLocalRotation(rotation);
	SetLocalScale(scale);

	SetTransform2DHiearchyDirty();
}

void Transform2D::SetLocalPosition(Vector2 position)
{
	if(position != m_localPosition)
	{
		SetTransform2DHiearchyDirty();
		m_localPosition = position;
	}
}

void Transform2D::SetLocalRotation(float rotation)
{
	if(rotation != m_localRotation)
	{
		SetTransform2DHiearchyDirty();
		m_localRotation = rotation;
	}	
}

void Transform2D::SetLocalScale(Vector2 scale)
{
	if(scale != m_localScale)
	{
		SetTransform2DHiearchyDirty();
		m_localScale = scale;
	}	
}

void Transform2D::AddRotation(float rotation)
{
	if(rotation != 0.f)
	{
		m_localRotation += rotation;
		SetTransform2DHiearchyDirty();
	}
}

void Transform2D::TranslatePosition(Vector2 position)
{
	if(position != Vector2::ZERO)
	{
		m_localPosition += position;
		SetTransform2DHiearchyDirty();
	}
}

//gets identity with point;
Matrix44 Transform2D::GetLocalTranslationMatrix()
{
	return Matrix44::MakeTranslation2D(m_localPosition);
}

//gets the rotation matrix using m_rotation.z
Matrix44 Transform2D::GetLocalRotationMatrixZ()
{
	return Matrix44::MakeRotationDegrees2D(m_localRotation);
}

//gets the scale matrix
Matrix44 Transform2D::GetLocalScaleMatrix()
{
	return Matrix44::MakeScale2D(m_localScale.x, m_localScale.y);
}

//gets complete matrix
Matrix44 Transform2D::GetWorldMatrix()
{
	if(m_isDirty == true)
	{
		m_transformMatrix = Matrix44::IDENTITY;

		if(m_parentTransform2D != nullptr)
		{
			m_transformMatrix.Append(m_parentTransform2D->GetWorldMatrix());
		}

		//translate
		m_transformMatrix.Append(GetLocalTranslationMatrix());

		//rotate
		m_transformMatrix.Append(Matrix44::MakeRotationMatrix(Vector3(0.f, 0.f, m_localRotation)));		

		//scale
		m_transformMatrix.Append(GetLocalScaleMatrix());		
	}	

	m_isDirty = false;
	return m_transformMatrix;
}

Vector2 Transform2D::GetWorldPosition()
{
	if(m_parentTransform2D != nullptr)
	{
		return GetWorldMatrix().GetPosition2D();
	}
	else
	{
		return GetLocalPosition();
	}
}

Vector2 Transform2D::GetWorldUp()
{
	return GetWorldMatrix().GetUp2D();
}

Vector2 Transform2D::GetWorldRight()
{
	return GetWorldMatrix().GetRight2D();
}

bool Transform2D::IsTransform2DDirty()
{
	return m_isDirty;
}

void Transform2D::SetTransform2DHiearchyDirty()
{
	m_isDirty = true;

	for (Transform2D* childTransform2D : m_childrenTransform2Ds)
	{
		childTransform2D->SetTransform2DHiearchyDirty();
	}
}

void Transform2D::AddChildTransform2D(Transform2D* childTransform2D)
{
	childTransform2D->m_parentTransform2D = this;
	m_childrenTransform2Ds.push_back(childTransform2D);
}

void Transform2D::RemoveChildTransform2D(Transform2D * child)
{
	for(int childIndex = 0; childIndex < (int)m_childrenTransform2Ds.size(); childIndex++)
	{
		if(m_childrenTransform2Ds[childIndex] == child)
		{
			m_childrenTransform2Ds[childIndex]->m_parentTransform2D = nullptr;
			m_childrenTransform2Ds[childIndex] = nullptr;
			m_childrenTransform2Ds.erase(m_childrenTransform2Ds.begin() + childIndex);
		}
	}
}

void Transform2D::CopyPositionRotationScaleFromTransform2D(const Transform2D& copyFrom)
{
	SetLocalPosition(copyFrom.m_localPosition);
	SetLocalRotation(copyFrom.m_localRotation);
	SetLocalScale(copyFrom.m_localScale);
}

void Transform2D::ResetPositionData()
{
	SetLocalPosition(Vector2::ZERO);
	SetLocalRotation(0.f);
	SetLocalScale(Vector2::ONE);

	m_transformMatrix = Matrix44::IDENTITY;
}