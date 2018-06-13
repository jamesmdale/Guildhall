#pragma once
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\Material.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\Transform.hpp"

class Renderable
{
public:
	Renderable();
	~Renderable();

	void AddMesh(Mesh* mesh);
	Mesh* GetMesh(int meshIndex) const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

	void SetModelMatrix(const Matrix44& model);
	Matrix44 GetModelMatrix();

	//convenience methods
	Shader* GetShader() const;
	int GetRenderSortLayer() const;

public:
	Matrix44 m_modelMatrix;
	std::vector<Mesh*> m_meshes;
	Material* m_material = nullptr;
	Transform* m_watch = nullptr;
};

