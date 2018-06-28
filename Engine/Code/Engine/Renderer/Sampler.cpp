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

bool Sampler::Create()
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

	// filtering; 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
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

	// setup wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );               

	glSamplerParameterfv( m_samplerHandle, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&Rgba::ConvertToVector4(Rgba::WHITE)); 

	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ); 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL ); 

	// filtering; 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
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