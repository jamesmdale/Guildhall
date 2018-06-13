#include "Engine\Renderer\Renderable.hpp"


Renderable::Renderable()
{
}

Renderable::~Renderable()
{
	for (int meshIndex = 0; meshIndex < (int)m_meshes.size(); ++meshIndex)
	{
		delete(m_meshes[meshIndex]);
		m_meshes[meshIndex] = nullptr;
	}
	m_meshes.clear();

	if (m_material->m_isInstance)
	{
		delete(m_material);
		m_material = nullptr;
	}
}

void Renderable::AddMesh(Mesh* mesh)
{
	m_meshes.push_back(mesh);
}

Mesh* Renderable::GetMesh(int meshIndex) const
{
	return m_meshes[meshIndex];
}

void Renderable::SetMaterial(Material* material)
{
	m_material = material;
}

Material* Renderable::GetMaterial() const
{
	return m_material;
}

void Renderable::SetModelMatrix(const Matrix44& model)
{
	m_modelMatrix = model;
}

Matrix44 Renderable::GetModelMatrix()
{
	if(m_watch == nullptr)
	{
		return m_modelMatrix;
	}

	m_modelMatrix = m_watch->GetWorldMatrix();

	return m_modelMatrix;
}

Shader* Renderable::GetShader() const
{
	return m_material->GetShader();
}

int Renderable::GetRenderSortLayer() const
{
	return GetShader()->GetSortingLayer();
}
