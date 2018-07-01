#pragma once
#include "Engine\Renderer\GlFunctions.hpp"

enum eSamplerType
{
	SAMPLER_TYPE_DEFAULT,
	SAMPLER_TYPE_LINEAR,
	SAMPLER_TYPE_SHADOW,
	NUM_SAMPLER_TYPES
};

class Sampler
{
public:
	Sampler();
	Sampler(const Sampler* copySampler);
	~Sampler();

	bool Create(eSamplerType type = SAMPLER_TYPE_DEFAULT);

	void Destroy();
	GLuint GetHandle() { return m_samplerHandle; };

private:
	bool CreateDefault();
	bool CreateLinearSampler();
	bool CreateShadowSampler();
	
public:
	GLuint m_samplerHandle;// OpenGL handle for this program, default 0
};