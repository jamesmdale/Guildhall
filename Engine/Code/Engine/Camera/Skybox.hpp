#pragma once
#include "Engine\Core\TextureCube.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Renderer\Shader.hpp"

class Skybox
{
public:
	Skybox(const char* image);
	~Skybox();

public:
	Matrix44 m_model;
	TextureCube* m_textureCube = nullptr;
	Mesh* m_mesh = nullptr;
	Shader* m_shader = nullptr;
};

