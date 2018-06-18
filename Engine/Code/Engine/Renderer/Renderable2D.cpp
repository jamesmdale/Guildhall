#include "Engine\Renderer\Renderable2D.hpp"

Renderable2D::Renderable2D()
{

}

Renderable2D::~Renderable2D()
{
	for (int dataIndex = 0; dataIndex < (int)m_renderableData.size(); ++dataIndex)
	{
		delete(m_renderableData[dataIndex]);
		m_renderableData[dataIndex] = nullptr;
	}	
	m_renderableData.clear();
}

void Renderable2D::AddRenderableData(int sortLayer, Mesh* mesh, Material* material)
{
	RenderableData* data = new RenderableData(sortLayer, mesh, material);
	m_renderableData.push_back(data);

	//cleanup
	data = nullptr;
}

void Renderable2D::AddRenderableData(RenderableData* data)
{
	m_renderableData.push_back(data);
}
	

Mesh* Renderable2D::GetMesh(int index) const
{
	return m_renderableData[index]->m_mesh;
}

Material* Renderable2D::GetMaterial(int index) const
{
	return m_renderableData[index]->m_material;
}

void Renderable2D::SetModelMatrix(const Matrix44& model)
{
	m_modelMatrix = model;
}

Matrix44 Renderable2D::GetModelMatrix()
{
	return m_modelMatrix;
}

Shader* Renderable2D::GetShader(int index) const
{
	return m_renderableData[index]->m_material->m_shader;
}

void Renderable2D::SetWidgetSortLayer(int sortLayer)
{
	m_widgetSortLayer = sortLayer;
}

