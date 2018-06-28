#pragma once
#include "Engine\Renderer\GlFunctions.hpp"
class Sampler
{
public:
	Sampler();
	Sampler(const Sampler* copySampler);
	~Sampler();
	
	bool Create();
	bool CreateShadowSampler();
	void Destroy();
	GLuint GetHandle(){return m_samplerHandle;};

public:
	GLuint m_samplerHandle;// OpenGL handle for this program, default 0
};