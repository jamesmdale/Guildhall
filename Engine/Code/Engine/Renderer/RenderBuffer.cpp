#include "Engine\Renderer\RenderBuffer.hpp"


RenderBuffer::RenderBuffer()
{
	buffer_handle = (GLuint)0;
}

bool RenderBuffer::CopyToGPU( size_t const byte_count, const void* data ) 
{
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (buffer_handle == NULL) {
		glGenBuffers( 1, &buffer_handle ); 
		GL_CHECK_ERROR();
	}

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer( GL_ARRAY_BUFFER, buffer_handle ); 
	GL_CHECK_ERROR();

	glBufferData( GL_ARRAY_BUFFER, byte_count, data, GL_DYNAMIC_DRAW ); 
	GL_CHECK_ERROR();

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	buffer_size = byte_count; 
	return true; 
}

RenderBuffer::~RenderBuffer()
{
	// cleanup for a buffer; 
	if (buffer_handle != NULL) {
		glDeleteBuffers( 1, &buffer_handle ); 
		GL_CHECK_ERROR();

		buffer_handle = NULL; 
	}
}
