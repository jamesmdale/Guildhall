#include "Engine\Renderer\Renderable.hpp"


Renderable::Renderable()
{
}

Renderable::~Renderable()
{
	delete(m_mesh);
	m_mesh = nullptr;

	/*delete(m_material);
	m_material = nullptr;*/
}

void Renderable::SetMesh(Mesh* mesh)
{
	if(m_mesh != nullptr)
	{
		delete(m_mesh);
		m_mesh = nullptr;		
	}
	m_mesh = mesh;
}

Mesh* Renderable::GetMesh() const
{
	return m_mesh;
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
