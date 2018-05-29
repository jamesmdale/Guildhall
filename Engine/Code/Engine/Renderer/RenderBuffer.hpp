#pragma once
#include "Engine\Renderer\GlFunctions.hpp"

class RenderBuffer
{
   public:
      RenderBuffer();   // initialize data
      virtual ~RenderBuffer();  // cleanup OpenGL resource 

      // copies data to the GPU
      bool CopyToGPU( size_t const byte_count, void const *data ); 
	  GLuint GetHandle() const {return buffer_handle;};

   public:
      GLuint buffer_handle;       // OpenGL handle to the GPU buffer, default = NULL; 
      size_t buffer_size;  // how many bytes are in this buffer, default = 0
};