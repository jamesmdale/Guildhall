#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\GlFunctions.hpp"

class FrameBuffer
{

public:
	FrameBuffer(); 
	~FrameBuffer();

	// should just update members
	// finalize does the actual binding
	void SetColorTarget( Texture* colorTarget ) {m_colorTarget = colorTarget;}; 
	void SetDepthStencilTarget( Texture* depthTarget ){m_depthStencilTarget = depthTarget;}; 
	GLuint GetHandle(){return m_handle;};

	// setups the the GL framebuffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	bool Finalize(); 

public:
	GLuint m_handle; 
	Texture* m_colorTarget; 
	Texture* m_depthStencilTarget; 
};

