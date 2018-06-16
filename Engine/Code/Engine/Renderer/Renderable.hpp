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

	//convenience methods
	Shader* GetShader() const;
	int GetRenderSortLayer() const;

public:
	std::vector<Mesh*> m_meshes;
	Material* m_material = nullptr;
	Transform* m_transform = nullptr; //always a child
};

