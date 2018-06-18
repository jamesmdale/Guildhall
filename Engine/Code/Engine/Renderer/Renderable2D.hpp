#pragma once
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Renderer\Material.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Core\Transform2D.hpp"
#include "Engine\Math\Vector3.hpp"
#include <vector>

struct RenderableData
{
	RenderableData(int renderableLayer, Mesh* renderableMesh, Material* renderableMaterial)
	{
		m_meshDrawOrder = renderableLayer;
		m_mesh = renderableMesh;
		m_material = renderableMaterial;		
	}

	~RenderableData()
	{
		delete(m_mesh);
		m_mesh = nullptr;

		if (m_material->m_isInstance)
		{
			delete(m_material);
		}
		m_material = nullptr;
	}

	int m_meshDrawOrder = 0; // a sort layer within the renderable itself
	Mesh*  m_mesh = nullptr;
	Material*  m_material = nullptr;;
};

class Renderable2D
{
public:
	Renderable2D();
	~Renderable2D();

	void AddRenderableData(int sortLayer, Mesh* mesh, Material* material);
	void AddRenderableData(RenderableData* data);

	//getters
	Mesh* GetMesh(int index) const;
	Material* GetMaterial(int index) const;
	Shader* GetShader(int index) const;
	Matrix44 GetModelMatrix();

	//setters
	void SetWidgetSortLayer(int sortLayer);
	void SetModelMatrix(const Matrix44& model);	

public:
	int m_widgetSortLayer = 0;
	Matrix44 m_modelMatrix;
	std::vector<RenderableData*> m_renderableData;
};
