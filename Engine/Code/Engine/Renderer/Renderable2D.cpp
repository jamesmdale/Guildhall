#include "Engine\Renderer\Renderable2D.hpp"

Renderable2D::Renderable2D()
{

}

Renderable2D::~Renderable2D()
{
	delete(m_mesh);
	m_mesh = nullptr;
}

void Renderable2D::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

Mesh* Renderable2D::GetMesh() const
{
	return m_mesh;
}

void Renderable2D::SetMaterial(Material* material)
{
	m_material = material;
}

Material* Renderable2D::GetMaterial() const
{
	return m_material;
}

void Renderable2D::SetModelMatrix(const Matrix44& model)
{
	m_modelMatrix = model;
}

Matrix44 Renderable2D::GetModelMatrix()
{
	if(m_watch == nullptr)
	{
		return m_modelMatrix;
	}

	m_modelMatrix = m_watch->GetWorldMatrix();

	return m_modelMatrix;
}

Shader* Renderable2D::GetShader() const
{
	return m_material->GetShader();
}

void Renderable2D::SetRender2DSortLayer(const int sortLayer)
{
	m_sortLayer = sortLayer;
}

int Renderable2D::GetRender2DSortLayer() const
{
	return m_sortLayer;
}


