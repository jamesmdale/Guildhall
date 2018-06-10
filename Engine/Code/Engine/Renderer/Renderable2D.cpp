#include "Engine\Renderer\Renderable2D.hpp"

Renderable2D::Renderable2D()
{

}

Renderable2D::~Renderable2D()
{
	for (int meshIndex = 0; meshIndex < (int)m_meshes.size(); ++meshIndex)
	{
		delete(m_meshes[meshIndex]);
		m_meshes[meshIndex] = nullptr;
	}	
	m_meshes.clear();
}
	

void Renderable2D::AddMesh(Mesh* mesh)
{
	m_meshes.push_back(mesh);
}

Mesh* Renderable2D::GetMesh(int meshIndex) const
{
	return m_meshes[meshIndex];
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


