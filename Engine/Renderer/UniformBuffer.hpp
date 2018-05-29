#pragma once
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Renderer\RenderBuffer.hpp"

class UniformBuffer
{
public:
	UniformBuffer::UniformBuffer(){};
	UniformBuffer::UniformBuffer(const size_t byte_count, const void* data);   // initialize data
	~UniformBuffer();  // cleanup OpenGL resource 

	// copy data to the CPU and dirty the buffer
	void SetCPUData( size_t byte_size, void const *data ); 

	// update the gpu buffer from the local cpu buffer if dirty
	// and clears the dirty flag
	void UpdateGPU(); 

	// sets the cpu and gpu buffers - clears the dirty flag
	void SetGPUData( size_t byte_size, void const *data ); 

	// gets a pointer to the cpu buffer (const - so does not dirty)
	void const* GetCPUBuffer() const; 

	// get a mutable pointer to the cpu buffer.  Sets the dirty flag
	// as it expects the user to change it.
	void* GetCPUBuffer(); 

	// get this buffer's size in bytes
	size_t GetSize() const; 

	GLuint GetHandle() const {return m_renderBuffer.GetHandle();};

	// let's me just set a structure, and it'll figure out the size
	template <typename T> 
	void set( T const &v ) 
	{ 
		SetCPUData( sizeof(T), &v ); 
	}

	//------------------------------------------------------------------------
	// get's a constant reference to the CPU buffer as known struct 
	// would get the same as saying (T*) uniform_buffer->get_cpu_buffer(); 
	template <typename T>
	T const* as() const
	{
		return (T const*) GetCPUBuffer(); 
	}

	//------------------------------------------------------------------------
	template <typename T>
	T* as() 
	{
		return (T*) GetCPUBuffer();
	}

public:
	RenderBuffer m_renderBuffer;       // OpenGL handle to the GPU buffer, default = NULL; 
	void* m_cpuBuffer = nullptr;  // how many bytes are in this buffer, default = 0
	bool m_isDirty;
};

//EXAMPLES:

//REMBER STD140
/*
The rules for std140 layout are covered quite well in the OpenGL specification (OpenGL 4.5, Section 7.6.2.2, page 137).
Among the most important is the fact that arrays of types are not necessarily tightly packed.
An array of floats in such a block will not be the equivalent to an array of floats in C/C++.
The array stride (the bytes between array elements) is always rounded up to the size of a vec4 (ie: 16-bytes).
So arrays will only match their C/C++ definitions if the type is a multiple of 16 bytes. 
*/


/*
struct good_a {
vec2 pos;      // starts at a 16-byte boundary (0)
vec2 vel;      // starts at 8-byte, but doesn't cross the 16-byte boundary
vec4 color;    // starts at 16-byte, good!
}; 

struct bad_a {
vec3 position; // good, starts at 0 bytes
vec3 velocity; // bad, starts at 12, and crosses 16 (ends at 24)
vec4 color;    // bad, starts at 24, and ends at 40... 
};

struct fixed_bad_a {
vec3 position;    // good, 0 bytes
float __pad0;     // good, 12, but doesn't cross 16
vec3 velocity;    // now starts at 16, good!
float __pad1;     // we're at 28, so pad us to 32
float4 color;     // starts a 32;
};

struct good_b {
mat4 view;        // crosses a boundary, but starts on a boundary (0)
mat4 proj;        // same, starting at 64
};
*/