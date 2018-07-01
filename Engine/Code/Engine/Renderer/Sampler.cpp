#include "Engine\Renderer\Sampler.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Math\Vector4.hpp"

Sampler::Sampler() 
	: m_samplerHandle(NULL)
{}

Sampler::Sampler(const Sampler* copySampler)
{
	m_samplerHandle = copySampler->m_samplerHandle;
}

Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create(eSamplerType type)
{
	bool success = false;

	switch (type)
	{
	case SAMPLER_TYPE_LINEAR:
		success = CreateLinearSampler();
		break;

	case SAMPLER_TYPE_SHADOW:
		success = CreateShadowSampler();
		break;

	default:
		success = CreateDefault();
		break;
	}

	return success;
}

bool Sampler::CreateDefault()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL) 
	{
		glGenSamplers( 1, &m_samplerHandle ); 
		if (m_samplerHandle == NULL) 
		{
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );						// Default: GL_LINEAR
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );						// Default: GL_LINEAR

	// level of detail 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000.f); 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MAX_LOD, 1000.f ); 

	return true; 
}

bool Sampler::CreateLinearSampler()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL)
	{
		glGenSamplers(1, &m_samplerHandle);
		if (m_samplerHandle == NULL)
		{
			return false;
		}
	}

	// setup wrapping
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );       // Default: GL_LINEAR
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);						// Default: GL_LINEAR

																									// level of detail 
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000.f);
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MAX_LOD, 1000.f);

	return true;
}

bool Sampler::CreateShadowSampler()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL) 
	{
		glGenSamplers( 1, &m_samplerHandle ); 
		if (m_samplerHandle == NULL) 
		{
			return false; 
		}
	}

	// wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );               

	//border and depth
	glSamplerParameterfv( m_samplerHandle, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&Rgba::ConvertToVector4(Rgba::WHITE)); 

	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ); 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL ); 

	// filtering
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );         // Default: GL_LINEAR
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );         // Default: GL_LINEAR

	// level of detail 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000.f); 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MAX_LOD, 1000.f ); 

	return true; 
}

void Sampler::Destroy()
{
	if (m_samplerHandle != NULL) 
	{
		glDeleteSamplers( 1, &m_samplerHandle ); 
		m_samplerHandle = NULL; 
	}
} 