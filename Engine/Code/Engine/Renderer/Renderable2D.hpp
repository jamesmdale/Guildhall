#pragma once
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Renderer\Material.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Core\Transform2D.hpp"

class Renderable2D
{
public:
	Renderable2D();
	~Renderable2D();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

	void SetModelMatrix(const Matrix44& model);
	Matrix44 GetModelMatrix();

	//convenience methods
	Shader* GetShader() const;

	void SetRender2DSortLayer(int sortLayer);
	int GetRender2DSortLayer() const;

public:
	Matrix44 m_modelMatrix;
	Mesh* m_mesh = nullptr;
	Material* m_material = nullptr;
	Transform2D* m_watch = nullptr;
	int m_sortLayer = 0;
};
