#include "Engine\Core\WindowsCommon.hpp"
#pragma comment (lib, "opengl32")
#include "Renderer.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include <cstddef>
#include "Engine\Window\Window.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\ShaderLoader.hpp"
#include "Engine\Renderer\MaterialLoader.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Profiler\Profiler.hpp"

#define LIGHT_BUFFER_BINDING_SLOT (1)
#define LIGHT_SPECULAR_BUFFER_BINDING_SLORT  (2)
static Renderer* g_theRenderer = nullptr;

// Four needed variables.  Globals or private members of Renderer are fine; 
void* gGLLibrary = NULL;    //HMODULE
void* gGLwnd = NULL;    //HWND window our context is attached to; 
void* gHDC = NULL;    //HDC our device context
void* gGLContext = NULL;    //HGLRC our rendering context; 

							//COMMON SHADERS
const std::string defaultShaderVS = R"(#version 420 core

uniform mat4 PROJECTION;
uniform mat4 VIEW; 
uniform mat4 MODEL;

in vec3 POSITION;
in vec4 COLOR;       // NEW - GLSL will use a Vector4 for this; 
in vec2 UV;         

out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.

void main() 
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * VIEW * MODEL * local_pos; 

   passColor = COLOR; // pass it on. 
   passUV = UV; 
   gl_Position = clip_pos; 
}
)";
const std::string defaultShaderFS = R"(#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor; // NEW, passed color
in vec2 passUV; 

out vec4 outColor; 
  
void main() 
{
   vec4 diffuse = texture( gTexDiffuse, passUV );
   
   // multiply is component-wise
   // so this gets (diff.x * passColor.x, ..., diff.w * passColor.w)
   outColor = diffuse * passColor;  
})";

const std::string invalidShaderVS = R"(// define the shader version (this is required)
#version 420 core

// Attributes - input to this shasder stage (constant as far as the code is concerned)
in vec3 POSITION;

// Entry point - required.  What does this stage do?
void main( void )
{
   // for now, we're going to set the 
   // clip position of this vertex to the passed 
   // in position. 
   // gl_Position is a "system variable", or have special 
   // meaning within the shader.
   gl_Position = vec4( POSITION, 1 ); 
}
)";
const std::string invalidShaderFS = R"(#version 420 core

// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{
   outColor = vec4( 1, 0, 1, 1 ); 
})";


Renderer* Renderer::CreateInstance()
{
	if (g_theRenderer == nullptr) 
	{
		g_theRenderer = new Renderer(); 
	}
	return g_theRenderer; 
}

Renderer* Renderer::GetInstance()
{
	return g_theRenderer; 
}

static HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}

//------------------------------------------------------------------------
// Creates a real context as a specific version (major.minor)
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (UINT i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			DWORD error = GetLastError();
			DebuggerPrintf( "Failed to set the format: %u", error ); 
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the rihgt format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}


//startup begins here

bool RenderStartup() 
{
	HWND hwnd = GetActiveWindow();
	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA( "opengl32.dll" ); 

	// Get the Device Context (DC) - how windows handles the interace to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC( hwnd );       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext( hdc ); 

	::wglMakeCurrent( hdc, temp_context ); 
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

							// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext( hdc, 4, 2 ); 

	// Set and cleanup
	::wglMakeCurrent( hdc, real_context ); 
	::wglDeleteContext( temp_context ); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	gGLwnd = hwnd;
	gHDC = hdc; 
	gGLContext = real_context; 

	return true; 
}

void Renderer::Shutdown()
{
	delete(g_theRenderer);
	g_theRenderer = nullptr;
}


TODO("Add to endframe");
void GLShutdown()
{
	wglMakeCurrent( (HDC)gHDC, NULL ); 

	::wglDeleteContext( (HGLRC)gGLContext ); 
	::ReleaseDC( (HWND)gGLwnd, (HDC)gHDC ); 

	gGLContext = NULL; 
	gHDC = NULL;
	gGLwnd = NULL; 

	::FreeLibrary( (HMODULE)gGLLibrary ); 
}


void Renderer::BeginFrame()
{
	UseShaderProgram(CreateOrGetShaderProgramFromPath("default")); //always bind default passthrough at beginframe
	DisableAllLights(); //Reset lighting presets
}

void Renderer::EndFrame()
{
	PROFILER_PUSH();

	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	CopyFrameBuffer(nullptr, &m_currentCamera->m_frameBufferOutput);
	SwapBuffers( (HDC)gHDC ); // Note: call this once at the end of each frame
}

void Renderer::LineWidth(float lineWidth)
{
	UNUSED(lineWidth);
	UNIMPLEMENTED();//glLineWidth(lineWidth);
}

void Renderer::Enable()
{
	glEnable(GL_BLEND);
	GL_CHECK_ERROR();

	glEnable(GL_LINE_SMOOTH);
	GL_CHECK_ERROR();
}

void Renderer::ClearColor(const Rgba& color) const
{
	glClearColor( color.r, color.g, color.b, color.a);
	GL_CHECK_ERROR();

	glClear(GL_COLOR_BUFFER_BIT);	
	GL_CHECK_ERROR();
}

void Renderer::EnableDepth(DepthComparisonType compareType, bool shouldWrite)
{
	//enable/disable the test
	glEnable(GL_DEPTH_TEST);
	GL_CHECK_ERROR();

	glDepthFunc(GetGLDepthComparison(compareType));
	GL_CHECK_ERROR();

	// enable/disable write
	glDepthMask( shouldWrite ? GL_TRUE : GL_FALSE ); 
	GL_CHECK_ERROR();
}

void Renderer::ClearDepth(const float& amount = 1.0f) const
{
	glDepthMask(GL_TRUE);
	GL_CHECK_ERROR();

	glClearDepthf(amount);
	GL_CHECK_ERROR();

	glClear(GL_DEPTH_BUFFER_BIT);
	GL_CHECK_ERROR();
}

void Renderer::DisableDepth()
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	EnableDepth( ALWAYS_DEPTH_TYPE, false ); 
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	GL_CHECK_ERROR();
}

void Renderer::DrawLine(const Vector2& startingPoint, const Vector2& endPoint)
{
	//TODO: refactor for drawmesh
	Rgba color = Rgba(255, 255, 255, 255);
	VertexPCU vertex[2];
	vertex[0] = VertexPCU(Vector3(startingPoint.x, startingPoint.y, 0.f), color, Vector2(0, 0));
	vertex[1] = VertexPCU(Vector3(endPoint.x, endPoint.y, 0.f), color, Vector2(0, 0));	

	DrawMeshImmediate(&vertex[0], 2, LINES_DRAW_PRIMITIVE);
}

void Renderer::DrawLine(const Vector3& startingPoint, const Vector3& endPoint)
{
	//TODO: refactor for drawmesh
	Rgba color = Rgba(255, 255, 255, 255);
	VertexPCU vertex[2];
	vertex[0] = VertexPCU(Vector3(startingPoint.x, startingPoint.y, startingPoint.z), color, Vector2(0, 0));
	vertex[1] = VertexPCU(Vector3(endPoint.x, endPoint.y, endPoint.z), color, Vector2(0, 0));	

	DrawMeshImmediate(&vertex[0], 2, LINES_DRAW_PRIMITIVE);
}

void Renderer::DrawLineWithColor(const Vector3& startingPoint, const Vector3& endPoint, const Rgba& color)
{
	VertexPCU vertex[2];
	vertex[0] = VertexPCU(Vector3(startingPoint.x, startingPoint.y, startingPoint.z), color, Vector2(0, 0));
	vertex[1] = VertexPCU(Vector3(endPoint.x, endPoint.y, endPoint.z), color, Vector2(0, 0));	

	DrawMeshImmediate(&vertex[0], 2, LINES_DRAW_PRIMITIVE);
}

void Renderer::DrawDottedDisc2WithColor(const Disc2& disc, const Rgba& color, const float& numSides)
{
	UNIMPLEMENTED();
	TODO("Add for Vector3");
	//TODO: refactor for drawmesh
	float degrees = 360.f/numSides;
	float sides = numSides;
	const int sidesInt = (int)sides;
	for(float i = 1; i < numSides + 1; i+=2) //skip every other line
	{
		float radiansStart = ConvertDegreesToRadians((float)i * degrees);
		float radiansEnd = ConvertDegreesToRadians((float)(i+1)*degrees);

		float startX = CalculateXPoint(disc.center.x, disc.radius, radiansStart);
		float startY = CalculateYPoint(disc.center.y, disc.radius, radiansStart);

		float endX = CalculateXPoint(disc.center.x, disc.radius, radiansEnd);
		float endY = CalculateYPoint(disc.center.y, disc.radius, radiansEnd);
		
		VertexPCU vertex[2];
		vertex[0] = VertexPCU(Vector3(startX, startY, 0), color, Vector2(0, 0));
		vertex[1] = VertexPCU(Vector3(endX, endY, 0), color, Vector2(0, 0));	
		DrawMeshImmediate(&vertex[0], 2, LINES_DRAW_PRIMITIVE);
	}	
}
//
//void Renderer::Translatef(const Vector2& center) const
//{
//	UNIMPLEMENTED();//glTranslatef(center.x, center.y, 0);
//}
//void Renderer::Rotatef(float angle, float x, float y, float z) const
//{
//	UNIMPLEMENTED();//glRotatef(angle, x, y, z);
//}
//void Renderer::Scalef(float x, float y, float z) const
//{
//	UNIMPLEMENTED();//glScalef(x, y, z);
//}

void Renderer::Blend() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	GL_CHECK_ERROR();
}
void Renderer::ResetBlend() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_CHECK_ERROR();
}

void Renderer::SetDrawMode(DrawModeFaceType face, DrawModeType mode)
{
	glPolygonMode(GetGLDrawFace(face), GetGLDrawMode(mode));
	GL_CHECK_ERROR();
}

void Renderer::SetDefaultDrawMode()
{
	glPolygonMode(FRONT_AND_BACK_FACE_MODE, FILL_DRAW_MODE);
	GL_CHECK_ERROR();
}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba& tint)
{
	Vector2 texCoordsAtMins = AABB2::ZERO_TO_ONE.mins;
	Vector2 texCoordsAtMaxs = AABB2::ZERO_TO_ONE.maxs;

	VertexPCU vertex[6];
	vertex[0] =  VertexPCU(Vector3(bounds.mins.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(Vector3(bounds.maxs.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[2] = VertexPCU(Vector3(bounds.maxs.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[3] = vertex[2];
	vertex[4] = VertexPCU(Vector3(bounds.mins.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[5] = vertex[0];

	DrawMeshImmediate(&vertex[0], 6, TRIANGLES_DRAW_PRIMITIVE);
}

TODO("Convert other draw methods to use matrixes so we can use transforms more easily");
void Renderer::DrawOrientedTexturedAABB(Matrix44& transformMatrix,
	const AABB2& bounds,
	Texture* texture, 
	const Rgba& tint,
	const AABB2& uvs)
{	
	Vector3 position = transformMatrix.GetPosition();
	SetTexture(*texture);

	Vector2 texCoordsAtMins = uvs.mins;
	Vector2 texCoordsAtMaxs = uvs.maxs;

	//position + (transformMatrixGetRight() * spriteDimensions.x) + (transformMatrix.GetUp() * spriteDimensions.y)
	VertexPCU vertex[6];
	vertex[0] =  VertexPCU(Vector3(bounds.mins.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(Vector3(bounds.maxs.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[2] = VertexPCU(Vector3(bounds.maxs.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[3] = vertex[2];
	vertex[4] = VertexPCU(Vector3(bounds.mins.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[5] = vertex[0];

	DrawMeshImmediate(&vertex[0], 6, TRIANGLES_DRAW_PRIMITIVE, transformMatrix);
}

//void Renderer::DrawOrientedTexturedAABB(const Vector3& position,
//	const AABB2& bounds,
//	Texture* texture, 
//	const Rgba& tint,
//	const AABB2& uvs)
//{	
//	SetTexture(*texture);
//
//	Vector2 texCoordsAtMins = uvs.mins;
//	Vector2 texCoordsAtMaxs = uvs.maxs;
//	int textureIndex = 0;
//
//	BindSampler(textureIndex, m_defaultSampler);
//
//	// Bind the texture
//	glActiveTexture( GL_TEXTURE0 + textureIndex );
//	glBindTexture(GL_TEXTURE_2D, (*m_currentTexture).m_textureID);
//
//	//position + (transformMatrixGetRight() * spriteDimensions.x) + (transformMatrix.GetUp() * spriteDimensions.y)
//	Vertex_3DPCU vertex[6];
//	vertex[0] =  Vertex_3DPCU(Vector3(bounds.mins.x, bounds.mins.y, 0.f) , tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
//	vertex[1] = Vertex_3DPCU(Vector3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
//	vertex[2] = Vertex_3DPCU(Vector3(bounds.maxs.x, bounds.maxs.y, 0.f), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
//
//	vertex[3] = vertex[2];
//	vertex[4] = Vertex_3DPCU(Vector3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
//	vertex[5] = vertex[0];
//
//	DrawMeshImmediate(&vertex[0], 6, GetGLDrawPrimitive(TRIANGLES), position);
//}

void Renderer::BindTexture(const Texture& texture, int index, Sampler* sampler) //for now always zero
{
	//m_currentTexture = texture;

	if(sampler == nullptr)
		BindSampler(m_defaultSampler, index);
	else
		BindSampler(sampler, index);

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + index );
	GL_CHECK_ERROR();

	glBindTexture(GL_TEXTURE_2D, (texture).m_textureID);
	GL_CHECK_ERROR();
}

void Renderer::BindTextureCube(TextureCube* textureCube, int index)
{
	BindSampler(m_defaultSampler, index);

	//Bind texturecube
	glActiveTexture(GL_TEXTURE0 + index);
	GL_CHECK_ERROR();

	glBindTexture(GL_TEXTURE_CUBE_MAP, (GLuint)textureCube->GetHandle());
	GL_CHECK_ERROR();
}

void Renderer::BindSampler(Sampler* sampler, int textureIndex)
{
	glBindSampler( textureIndex, sampler->GetHandle() ); 
}

void Renderer::DrawCube(const Vector3& center, const Vector3& dimensions,
	const Rgba& tint,
	const AABB2& uvTop,
	const AABB2& uvSide,
	const AABB2& uvBottom)
{
	float xVal = 0.f;
	float yVal = 0.f;
	float zVal = 0.f;

	if(dimensions.x != 0.f)
		xVal = dimensions.x/2.f;

	if(dimensions.y != 0.f)
		yVal = dimensions.y/2.f;

	if(dimensions.z != 0.f)
		zVal = dimensions.z/2.f;

	/*int indices [36] = {};*/
	VertexPCU vertex[36];

	Vector2 texCoordsAtMins = uvSide.mins;
	Vector2 texCoordsAtMaxs = uvSide.maxs;
	//front face
	vertex[0] =  VertexPCU(Vector3(center.x - xVal, center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[2] = VertexPCU(Vector3(center.x + xVal, center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[3] = vertex[2];
	vertex[4] = VertexPCU(Vector3(center.x + xVal,  center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[5] = vertex[0];

	//right face
	vertex[6] =  VertexPCU(Vector3(center.x + xVal, center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[7] = VertexPCU(Vector3(center.x + xVal, center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[8] = VertexPCU(Vector3(center.x + xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[9] = vertex[8];
	vertex[10] = VertexPCU(Vector3(center.x + xVal,  center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[11] = vertex[6];

	//back face
	vertex[12] =  VertexPCU(Vector3(center.x + xVal, center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[13] = VertexPCU(Vector3(center.x + xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[14] = VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[15] = vertex[14];
	vertex[16] = VertexPCU(Vector3(center.x - xVal,  center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[17] = vertex[12];

	//left face
	vertex[18] =  VertexPCU(Vector3(center.x - xVal, center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[19] = VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[20] = VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[21] = vertex[20];
	vertex[22] = VertexPCU(Vector3(center.x - xVal,  center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[23] = vertex[18];


	texCoordsAtMins = uvTop.mins;
	texCoordsAtMaxs = uvTop.maxs;
	//top face
	vertex[24] =  VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[25] = VertexPCU(Vector3(center.x - xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[26] = VertexPCU(Vector3(center.x + xVal, center.y + yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[27] = vertex[26];
	vertex[28] = VertexPCU(Vector3(center.x + xVal,  center.y + yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[29] = vertex[24];

	texCoordsAtMins = uvBottom.mins;
	texCoordsAtMaxs = uvBottom.maxs;
	//bottom face
	vertex[30] =  VertexPCU(Vector3(center.x - xVal, center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[31] = VertexPCU(Vector3(center.x - xVal, center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[32] = VertexPCU(Vector3(center.x + xVal, center.y - yVal, center.z + zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[33] = vertex[32];
	vertex[34] = VertexPCU(Vector3(center.x + xVal,  center.y - yVal, center.z - zVal), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[35] = vertex[30];

	DrawMeshImmediate(&vertex[0], 36, TRIANGLES_DRAW_PRIMITIVE);

	//DrawMeshImmediateWithIndices(&vertex[0], )
}

void Renderer::DrawSkybox(Skybox* skybox)
{
	SetShader(skybox->m_shader);

	BindTextureCube(skybox->m_textureCube, 8);

	DrawMesh(skybox->m_mesh, skybox->m_model);	
}

void Renderer::BindMeshToProgram(ShaderProgram* program, Mesh* mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo->GetHandle());
	GL_CHECK_ERROR();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo->GetHandle());
	GL_CHECK_ERROR();

	int vertexStride = mesh->GetVertexStride();

	GLuint programHandle = program->GetHandle();
    int attributeCount = mesh->m_layout->GetAttributeCount();

	for(int attributeIndex = 0; attributeIndex < attributeCount; attributeIndex++)
	{
		const VertexAttribute& attribute = mesh->m_layout->GetAttribute(attributeIndex);
		GLint bind = glGetAttribLocation(programHandle, attribute.name.c_str());
		GL_CHECK_ERROR();

		if (bind >= 0) {
			glEnableVertexAttribArray( bind );
			GL_CHECK_ERROR();

			// be sure mesh and program are bound at this point
			// as this links them together
			glVertexAttribPointer( bind,
					attribute.elementCount,      // how many? 
					GetGLRenderDataType(attribute.type),  // what are they 
					attribute.isNormalized,      // are they normalized?
					vertexStride,          // vertex size?
					(GLvoid*)(int*)attribute.memberOffset // data offset from start of vertex
				);     
			GL_CHECK_ERROR();
		}
	}
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint)
{
	SetTexture(texture);

	VertexPCU vertex[6];
	vertex[0] = VertexPCU(Vector3(bounds.mins.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(Vector3(bounds.maxs.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[2] = VertexPCU(Vector3(bounds.maxs.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[3] = vertex[2];
	vertex[4] = VertexPCU(Vector3(bounds.mins.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[5] = vertex[0];

	DrawMeshImmediate(&vertex[0], 6, TRIANGLES_DRAW_PRIMITIVE);
}


void Renderer::DrawTexturedAABB(Matrix44& transformMatrix, const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint)
{
	SetTexture(texture);

	VertexPCU vertex[6];
	vertex[0] =  VertexPCU(Vector3(bounds.mins.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(Vector3(bounds.maxs.x, bounds.mins.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertex[2] = VertexPCU(Vector3(bounds.maxs.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	vertex[3] = vertex[2];
	vertex[4] = VertexPCU(Vector3(bounds.mins.x, bounds.maxs.y, 0), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[5] = vertex[0];

	DrawMeshImmediate(&vertex[0], 6, TRIANGLES_DRAW_PRIMITIVE, transformMatrix);
}

void Renderer::DrawTriangle(const Vector3& aPosition, const Vector3& bPosition, const Vector3& cPosition, const Rgba& tint)
{
	Vector2 texCoordsAtMins = AABB2::ZERO_TO_ONE.mins;
	Vector2 texCoordsAtMaxs = AABB2::ZERO_TO_ONE.maxs;

	VertexPCU vertex[3];
	vertex[0] = VertexPCU(aPosition, tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertex[1] = VertexPCU(bPosition, tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertex[2] = VertexPCU(cPosition, tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	DrawMeshImmediate(&vertex[0], 3, TRIANGLES_DRAW_PRIMITIVE);
}

Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	std::map<std::string, Texture*>::iterator texture = m_loadedTextures.find(imageFilePath);
	if(texture == m_loadedTextures.end())
	{
		Texture* newTexturePtr = new Texture(imageFilePath);
		m_loadedTextures[imageFilePath] = newTexturePtr;

		return newTexturePtr;
	}
	else
	{
		return m_loadedTextures[imageFilePath];
	}
}

Texture* Renderer::CreateOrGetTexture(Image& image)
{
	std::map<std::string, Texture*>::iterator texture = m_loadedTextures.find(image.GetImagePath());
	if(texture == m_loadedTextures.end())
	{
		Texture* newTexturePtr = new Texture(image);
		m_loadedTextures[image.GetImagePath()] = newTexturePtr;

		return newTexturePtr;
	}
	else
	{
		return m_loadedTextures[image.GetImagePath()];
	}
}

BitmapFont* Renderer::CreateOrGetBitmapFont(const char* bitmapFontName)
{
	std::map<std::string, BitmapFont*>::iterator font = m_loadedFonts.find(bitmapFontName);
	if(font == m_loadedFonts.end())
	{
		std::string imageFilePath = "Data/Fonts/" + (std::string)bitmapFontName + ".png";
		Texture* bitmapTexture = CreateOrGetTexture(imageFilePath);
		SpriteSheet* bitmapSpriteSheet = new SpriteSheet(*bitmapTexture, 16, 16);

		BitmapFont* newFontPointer = new BitmapFont(bitmapFontName, *bitmapSpriteSheet, 1.f);
		m_loadedFonts[bitmapFontName] = newFontPointer;

		return newFontPointer;
	}
	else
	{
		return m_loadedFonts[bitmapFontName];
	}
}

Image * Renderer::CreateOrGetImage(const std::string imageFilePath)
{
	std::map<std::string, Image*>::iterator image = m_loadedImages.find(imageFilePath);
	if (image == m_loadedImages.end())
	{
		Image* newImage = new Image(imageFilePath);
		m_loadedImages[imageFilePath] = newImage;

		return newImage;
	}
	else
	{
		return m_loadedImages[imageFilePath];
	}
}

Shader * Renderer::CreateOrGetShader(const std::string& shaderName)
{
	std::map<std::string, Shader*>::iterator shader = m_loadedShaders.find(shaderName);
	if(shader == m_loadedShaders.end())
	{
		std::string shaderFilePath = "Data/Shaders/XML/" + shaderName + ".xml";
		Shader* newShader = ShaderLoader::LoadShaderFromXML(shaderFilePath);
		m_loadedShaders[shaderName] = newShader;
		return newShader;
	}
	else
	{
		return m_loadedShaders[shaderName];
	}
}

Material* Renderer::CreateOrGetMaterial(const std::string& materialName)
{
	std::map<std::string, Material*>::iterator material = m_loadedMaterials.find(materialName);
	if(material == m_loadedMaterials.end())
	{
		std::string materialFilePath = "Data/Materials/" + materialName + ".xml";
		Material* newMaterial = MaterialLoader::LoadMaterialFromXML(materialFilePath);
		m_loadedMaterials[materialName] = newMaterial;

		return newMaterial;
	}
	else
	{
		return m_loadedMaterials[materialName];
	}
}

Texture* Renderer::CreateRenderTarget(int width, int height, TextureFormatType format)
{
	Texture* texture = new Texture();

	texture->CreateRenderTarget(width, height, format);
	
	return texture;
}

Texture* Renderer::CreateDepthStencilTarget(int width, int height)
{
	return CreateRenderTarget(width,height, TEXTURE_FORMAT_D24S8);
}

Texture * Renderer::GetDefaultDepthStencilTarget()
{
	return m_defaultDepthTarget;
}

Texture * Renderer::GetDefaultRenderTarget()
{
	return m_defaultColorTarget;
}

bool Renderer::CopyFrameBuffer(FrameBuffer* destination, FrameBuffer* source)
{
	// we need at least the src.
	if (source == nullptr) {
		return false; 
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint srcFBO = source->GetHandle();
	GLuint dstFBO = NULL; 
	if (destination != nullptr) {
		dstFBO = destination->GetHandle(); 
	}

	// can't copy onto ourselves
	if (dstFBO == srcFBO) {
		return false; 
	}
	GL_CHECK_ERROR();

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer( GL_READ_FRAMEBUFFER, srcFBO ); 
	GL_CHECK_ERROR();

	GLenum target = GL_COLOR_ATTACHMENT0 + 0;
	// Update target bindings
	glDrawBuffers( 1, &target ); 
	GL_CHECK_ERROR();

	// what are we copying to?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dstFBO ); 
	GL_CHECK_ERROR();

	TODO("Later you might want to legitimately get Framebuffer widths according to the target sizes");
	// blit it over - get the size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not

	/*int width = source->get_width();     
	int height = source->get_height(); */
	int width = (int)Window::GetInstance()->m_clientWidth;
	int height = (int)Window::GetInstance()->m_clientHeight;

	// Copy it over
	glBlitFramebuffer( 0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

	// Make sure it succeeded
	GL_CHECK_ERROR(); 

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 
	GL_CHECK_ERROR();

	return GLSucceeded();
}

void Renderer::SetTexture(const Texture& texture)
{
	BindTexture(texture, 0);
}

void Renderer::SetTexture(const Texture& texture, int index)
{
	BindTexture(texture, index);
}

void Renderer::SetCamera(Camera* camera)
{
	if (camera == nullptr) {
		camera = m_defaultCamera; 
	}

	camera->GetView();
	camera->m_frameBufferOutput.Finalize(); // make sure the framebuffer is finished being setup; 
    glViewport(0, 0, camera->m_viewPortDimensions.x, camera->m_viewPortDimensions.y); //set dimensions of view port
	m_currentCamera = camera;
}

void Renderer::DrawMeshImmediate(const VertexPCU* verts, int const numVerts, int drawPrimitive, const Matrix44& modelMatrix)
{
	m_defaultMesh->SetDrawInstructions((DrawPrimitiveType)drawPrimitive, 0, false);
	m_defaultMesh->ClearVertices();
	m_defaultMesh->SetVertices(numVerts, verts);

	DrawMesh(m_defaultMesh, modelMatrix);
}

TODO("Complete mesh immediate with indices.  Maybe consolidate into one method with a bool for indices 'True/False'");
void Renderer::DrawMeshImmediateWithIndices(const VertexPCU* verts, int* indices, int const numVerts, int const numIndices, int drawPrimitive)
{
	m_defaultMesh->SetDrawInstructions((DrawPrimitiveType)drawPrimitive, 0, true);
	m_defaultMesh->SetIndices(sizeof(int), numIndices, indices);
	m_defaultMesh->SetVertices(numVerts, verts);

	DrawMesh(m_defaultMesh, Matrix44::IDENTITY);
}

void Renderer::DrawMesh(Mesh* mesh, const Matrix44& modelMatrix)
{
	//int programHandle = BindShaderProgram(m_currentShader.m_program);
	BindRenderState(m_currentShader->m_state);
	BindMeshToProgram(m_currentShader->m_program, mesh);
	int programHandle = m_currentShader->m_program->GetHandle();

	SetMatrix44Uniform(programHandle, "PROJECTION", m_currentCamera->m_projMatrix);
	SetMatrix44Uniform(programHandle, "VIEW", m_currentCamera->m_viewMatrix);
	SetMatrix44Uniform(programHandle, "MODEL", modelMatrix);

	//pass game time to shader
	float value = (float)GetMasterClock()->GetRunningTimeInSeconds();
	SetFloatUniform(programHandle, "TIME", value);

	m_lightBuffer->UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 
		LIGHT_BUFFER_BINDING_SLOT, 
		m_lightBuffer->GetHandle() );
	GL_CHECK_ERROR();

//	LightBuffer* lightPtr = m_lightBuffer->as<LightBuffer>();

	m_specularLightBuffer->UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 
		LIGHT_SPECULAR_BUFFER_BINDING_SLORT, 
		m_specularLightBuffer->GetHandle() );
	GL_CHECK_ERROR();

//	SpecularLightBuffer* specLightPtr = m_specularLightBuffer->as<SpecularLightBuffer>();

	// Sometime before the draw call
	glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->m_frameBufferOutput.GetHandle());
	GL_CHECK_ERROR();


	if(mesh->m_drawInstruction.m_isUsingIndices)
	{
		glDrawElements(GetGLDrawPrimitive(mesh->m_drawInstruction.m_primitiveType), mesh->m_ibo->m_indexCount, GL_UNSIGNED_INT, 0);
		GL_CHECK_ERROR();
	}
	else
	{
		glDrawArrays( GetGLDrawPrimitive(mesh->m_drawInstruction.m_primitiveType), 0, mesh->m_vbo->m_vertexCount );
		GL_CHECK_ERROR();
	}
	
}

//TYPE SPECIFIC STUFF

void Renderer::DrawText2D(const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont* font)
{
	//glyph aspect is currently always 1

	float cellWidth = cellHeight * (font->m_baseAspect * aspectScale);
	//float stringWidth = font->GetStringWidth(asciiText, cellHeight, aspectScale);
	Texture fontTexture = *font->m_spriteSheet.GetSpriteSheetTexture();
	for(size_t charIndex = 0; charIndex < asciiText.length(); charIndex++)
	{	
		AABB2 uvs = font->GetUVsForGlyph(asciiText.at(charIndex));
		AABB2 drawBounds = AABB2(Vector2(drawMins.x + (cellWidth * charIndex), drawMins.y), Vector2(drawMins.x + (cellWidth * (charIndex + 1)), drawMins.y + cellHeight));
		DrawTexturedAABB(drawBounds, fontTexture, Vector2(uvs.mins.x, uvs.maxs.y), Vector2(uvs.maxs.x, uvs.mins.y), tint); 
	}
}

void Renderer::DrawText2DCentered(const Vector2& drawCenterPoint, const std::string& asciiText, float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont* font)
{
	float cellWidth = cellHeight * (font->m_baseAspect * aspectScale);
	//float stringWidth = font->GetStringWidth(asciiText, cellHeight, aspectScale);
	Texture fontTexture = *font->m_spriteSheet.GetSpriteSheetTexture();
	float offsetFromCenterX = (asciiText.length() * cellWidth) * .5f;
	float offsetFromCenterY = cellHeight * .5f;
	
	Vector2 drawMins = Vector2(drawCenterPoint.x - offsetFromCenterX, drawCenterPoint.y - offsetFromCenterY);

	for(size_t charIndex = 0; charIndex < asciiText.length(); charIndex++)
	{	
		AABB2 uvs = font->GetUVsForGlyph(asciiText.at(charIndex));
		AABB2 drawBounds = AABB2(Vector2(drawMins.x + (cellWidth * charIndex), drawMins.y), Vector2(drawMins.x + (cellWidth * (charIndex + 1)), drawMins.y + cellHeight));
		DrawTexturedAABB(drawBounds, fontTexture, Vector2(uvs.mins.x, uvs.maxs.y), Vector2(uvs.maxs.x, uvs.mins.y), tint); 
	}
}

void Renderer::DrawOrientedText2DCentered(Matrix44& transformMatrix, const std::string& asciiText, float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont * font)
{
	float cellWidth = cellHeight * (font->m_baseAspect * aspectScale);
	
	Texture fontTexture = *font->m_spriteSheet.GetSpriteSheetTexture();
	
	Vector3 drawCenterPoint = transformMatrix.GetPosition();

	//float offsetFromCenterX = (asciiText.length() * cellWidth) * .5f;
	//float offsetFromCenterY = cellHeight * .5f;
	//float stringWidth = font->GetStringWidth(asciiText, cellHeight, aspectScale);
	//Vector2 drawMins = Vector2(drawCenterPoint.x - offsetFromCenterX, drawCenterPoint.y - offsetFromCenterY);

	Vector2 drawMins = Vector2(-0.5f, 1.5f);

	for(size_t charIndex = 0; charIndex < asciiText.length(); charIndex++)
	{	
		AABB2 uvs = font->GetUVsForGlyph(asciiText.at(charIndex));
		AABB2 drawBounds = AABB2(Vector2(drawMins.x + (cellWidth * charIndex), drawMins.y), Vector2(drawMins.x + (cellWidth * (charIndex + 1)), drawMins.y + cellHeight));
	
		DrawOrientedTexturedAABB(transformMatrix, drawBounds, &fontTexture, tint, uvs); 
	}
}

void Renderer::PostStartup()
{
	// m_defaultVAO is a GLuint member variable
	glGenVertexArrays( 1, &m_defaultVAO ); 
	glBindVertexArray( m_defaultVAO );

	GL_CHECK_ERROR();

	//create default sampler
	m_defaultSampler = new Sampler();
	m_defaultSampler->Create();

	//load default texture
	Image defaultImage = Image(IntVector2(1,1), Rgba::WHITE, "default");
	Image normalImage = Image(IntVector2(1,1), Rgba::NORMAL_MAP_FLAT, "normal");

	m_defaultTexture = CreateOrGetTexture(defaultImage);
	m_defaultTexture = CreateOrGetTexture(normalImage);
	
	SetTexture(*m_defaultTexture);
	
	//load fonts
	CreateOrGetBitmapFont("SquirrelFixedFont");

	//load default mesh
	m_defaultMesh = new Mesh();

	//load default shaders
	m_defaultShader = new Shader();
	m_currentShader = new Shader();

	m_defaultShader->SetProgram(CreateOrGetShaderProgramFromPath( "default" )); 
	m_currentShader = m_defaultShader;  

	//Shader program test
	CreateOrGetShaderProgramFromPath("invalid");

	//load shader programs
	CreateOrGetShaderProgramFromPath("Data/Shaders/fromFileTest"); //TEST FOR LOADING FROM FILES (TO BE USED IN CONJUNCTION WITH RELOAD SHADERS)

	Enable();
	ResetBlend();	

	// create our output textures
	m_defaultColorTarget = CreateRenderTarget( (int)Window::GetInstance()->m_clientWidth, 
		(int)Window::GetInstance()->m_clientHeight );
	m_defaultDepthTarget = CreateRenderTarget( (int)Window::GetInstance()->m_clientWidth, 
		(int)Window::GetInstance()->m_clientHeight, 
		TEXTURE_FORMAT_D24S8); 

	// setup the initial camera
	m_defaultCamera = new Camera();
	m_defaultCamera->SetColorTarget( m_defaultColorTarget ); 
	m_defaultCamera->SetDepthStencilTarget( m_defaultDepthTarget ); 

	m_effectCamera = new Camera();
	m_effectCamera->SetColorTarget( m_defaultColorTarget ); 

	// set our default camera to be our current camera
	SetCamera(nullptr); 

	//initialize our UBOS and set them to default values
	LightBuffer* lightBuffer = new LightBuffer();
	lightBuffer->SetBufferToDefault();
	m_lightBuffer = new UniformBuffer(sizeof(LightBuffer), lightBuffer);

	lightBuffer = nullptr;

	SpecularLightBuffer* specLightBuffer = new SpecularLightBuffer();
	specLightBuffer->SetBufferToDefault();
	m_specularLightBuffer = new UniformBuffer(sizeof(SpecularLightBuffer), specLightBuffer);

	specLightBuffer = nullptr;
}

void Renderer::ReloadShaders()
{
	for (std::map<std::string, ShaderProgram*>::iterator shaderIterator = m_loadedShaderPrograms.begin(); shaderIterator!=m_loadedShaderPrograms.end(); ++shaderIterator)
	{	
		TODO("Might want a better way of excluding built in shaders in the future if there are more than just a couple");
		if(shaderIterator->first != "default" && shaderIterator->first != "invalid") //exclude built in shaders for now. 
		{
			shaderIterator->second->LoadFromFiles(shaderIterator->first.c_str());
		}		
	}

	DebuggerPrintf("Reloaded shaders!");
}

//TODO: Delete this method after SD2:A01
void Renderer::LoadErrorShader()
{
	UseShaderProgram(CreateOrGetShaderProgramFromPath("Data/Shaders/errorTest"));
}

void Renderer::CopyTexture(Texture* destination, Texture* source)
{
	FrameBuffer sourceBuffer;
	FrameBuffer destinationBuffer;

	sourceBuffer.m_colorTarget = source;
	destinationBuffer.m_colorTarget = destination;

	CopyFrameBuffer(&destinationBuffer, &sourceBuffer);
}

void Renderer::SetAmbientLight(const Rgba& color, float intensity)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	buffer->m_ambientColorAndIntensity = Vector4(color);
	buffer->m_ambientColorAndIntensity.w = intensity;

	buffer = nullptr;
}

void Renderer::SetAmbientLightIntensity(float intensity)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	buffer->m_ambientColorAndIntensity.w = intensity;

	buffer = nullptr;
}

void Renderer::DisableAllLights()
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	buffer->SetBufferToDefault();

	buffer = nullptr;
}

void Renderer::ResetSpecularBuffer()
{
	//reset spec buffer
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();
	buffer->SetBufferToDefault();

	buffer = nullptr;
}

void Renderer::EnablePointLight(int index, const Vector3& position, const Rgba& color, float intensity, const Vector3& attenuationConstants)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();

	buffer->m_lights[index].m_lightPosition = position;
	buffer->m_lights[index].m_colorAndIntensity = Rgba::ConvertToVector4(color);
	buffer->m_lights[index].m_colorAndIntensity.w = intensity;
	buffer->m_lights[index].m_attenuation = attenuationConstants;
	buffer->m_lights[index].m_lightForward = Vector3::FORWARD;
	buffer->m_lights[index].m_lightDirectionFactor = 0.f; //0 for point light. (0-1] for directed light
	buffer->m_lights[index].m_lightInnerAngle = CosDegrees(360.f/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_lightOuterAngle = CosDegrees(360.f/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_isShadowCasting = 0.f;
	buffer->m_lights[index].m_viewProjectionMatrix = Matrix44::IDENTITY;

	buffer = nullptr;
}

void Renderer::EnableDirectionalLight(int index, const Vector3& position, const Rgba& color, float intensity, const Vector3& attenuationConstants, const Vector3& forward, float directionFactor, float innerAngle, float outerAngle, float isShadowCasting, const Matrix44& viewProjectionMatrix)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();

	buffer->m_lights[index].m_lightPosition = position;
	buffer->m_lights[index].m_colorAndIntensity = Rgba::ConvertToVector4(color);
	buffer->m_lights[index].m_colorAndIntensity.w = intensity;
	buffer->m_lights[index].m_attenuation = attenuationConstants;
	buffer->m_lights[index].m_lightForward = forward;
	buffer->m_lights[index].m_lightDirectionFactor = directionFactor; //0 for point light. (0-1] for directed light
	buffer->m_lights[index].m_lightInnerAngle = CosDegrees(innerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_lightOuterAngle = CosDegrees(outerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_isShadowCasting = isShadowCasting;
	buffer->m_lights[index].m_viewProjectionMatrix = viewProjectionMatrix;

	buffer = nullptr;
}

void Renderer::EnableConeLight(int index, const Vector3& position, Rgba & color, float intensity, const Vector3& attenuationConstants, const Vector3& forward, float directionFactor, float innerAngle, float outerAngle)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();

	buffer->m_lights[index].m_lightPosition = position;
	buffer->m_lights[index].m_colorAndIntensity = color.Rgba::ConvertToVector4(color);
	buffer->m_lights[index].m_colorAndIntensity.w = intensity;
	buffer->m_lights[index].m_attenuation = attenuationConstants;
	buffer->m_lights[index].m_lightForward = forward;
	buffer->m_lights[index].m_lightDirectionFactor = directionFactor; //0 for point light. (0-1] for directed light
	buffer->m_lights[index].m_lightInnerAngle = CosDegrees(innerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_lightOuterAngle = CosDegrees(outerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_isShadowCasting = 0.f;
	buffer->m_lights[index].m_viewProjectionMatrix = Matrix44::IDENTITY;

	buffer = nullptr;
}

void Renderer::EnableLight(int index, const Vector3 & position, const Rgba & color, float intensity, const Vector3 & attenuationConstants, const Vector3 & forward, float directionFactor, float innerAngle, float outerAngle, float isShadowCasting, const Matrix44& viewProjectionMatrix)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();

	buffer->m_lights[index].m_lightPosition = position;
	buffer->m_lights[index].m_colorAndIntensity = color.Rgba::ConvertToVector4(color);
	buffer->m_lights[index].m_colorAndIntensity.w = intensity;
	buffer->m_lights[index].m_attenuation = attenuationConstants;
	buffer->m_lights[index].m_lightForward = forward;
	buffer->m_lights[index].m_lightDirectionFactor = directionFactor; //0 for point light. (0-1] for directed light
	buffer->m_lights[index].m_lightInnerAngle = CosDegrees(innerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_lightOuterAngle = CosDegrees(outerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_isShadowCasting = isShadowCasting;
	buffer->m_lights[index].m_viewProjectionMatrix = viewProjectionMatrix;
	buffer = nullptr;
}

void Renderer::EnableLight(int index, const Light& light)
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();

	buffer->m_lights[index].m_lightPosition = light.m_lightPosition;
	buffer->m_lights[index].m_colorAndIntensity = light.m_colorAndIntensity;
	buffer->m_lights[index].m_attenuation = light.m_attenuation;
	buffer->m_lights[index].m_lightForward = light.m_lightForward;
	buffer->m_lights[index].m_lightDirectionFactor = light.m_lightDirectionFactor; //0 for point light. (0-1] for directed light
	buffer->m_lights[index].m_lightInnerAngle = CosDegrees(light.m_lightInnerAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_lightOuterAngle = CosDegrees(light.m_lightOuterAngle/2.f); //360 to 360 for light in all directions
	buffer->m_lights[index].m_isShadowCasting = light.m_isShadowCasting;
	buffer->m_lights[index].m_viewProjectionMatrix = light.m_viewProjectionMatrix;
	buffer = nullptr;
}

void Renderer::SetLightBufferFromArray(std::vector<Light*> lights)
{
	DisableAllLights();

	for (int lightIndex = 0; lightIndex < (int)lights.size(); lightIndex++)
	{
		EnableLight(lightIndex, *lights[lightIndex]);
	}
}


void Renderer::SetSpecularConstants(float specularAmount, float specularPower)
{
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();

	buffer->m_specularAmount = specularAmount;
	buffer->m_specularPower = specularPower;

	buffer = nullptr;
}

void Renderer::SetSpecularAmount(float specularAmount)
{
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();

	buffer->m_specularAmount = specularAmount;

	buffer = nullptr;
}

void Renderer::SetSpecularPower(float specularPower)
{
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();

	buffer->m_specularPower = specularPower;

	buffer = nullptr;
}

Vector4 Renderer::GetAmbientLight()
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	Vector4 color = buffer->m_ambientColorAndIntensity;
	buffer = nullptr;

	return color;
}

Rgba Renderer::GetAmbientLightAsRGBA()
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	Vector4 ambientLight = ClampVector4Uniform(buffer->m_ambientColorAndIntensity, Vector4::ZERO, Vector4::ONE);
	buffer = nullptr;

	return Rgba(ambientLight);
}

float Renderer::GetAmbientLightIntensity()
{
	LightBuffer* buffer = m_lightBuffer->as<LightBuffer>();
	float intensity = buffer->m_ambientColorAndIntensity.w;
	buffer = nullptr;

	return intensity;
}

float Renderer::GetSpecularAmount()
{
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();
	float specAmount = buffer->m_specularAmount;
	buffer = nullptr;

	return specAmount;
}

float Renderer::GetSpecularPower()
{
	SpecularLightBuffer* buffer = m_specularLightBuffer->as<SpecularLightBuffer>();
	float specAmount = buffer->m_specularPower;

	return specAmount;
}

//uniform binds --------------------------------------------------------------------------------------------------

TODO("Make generic method for all uniform binding.");
bool Renderer::SetMatrix44Uniform(int programHandle, std::string uniformName, const Matrix44& input)
{
	bool success = false;

	//bind uniforms
	GLint matrixBind = glGetUniformLocation(programHandle, uniformName.c_str());


	if (matrixBind >= 0)
	{		
		glUniformMatrix4fv( matrixBind, 1, GL_FALSE, (GLfloat*)&input );
		
		success = true;
	}

	GL_CHECK_ERROR();
	return success;
}

bool Renderer::SetFloatUniform(int programHandle, std::string uniformName, float input)
{
	bool success = false;

	//bind uniforms
	GLint floatBind = glGetUniformLocation(programHandle, uniformName.c_str());

	if (floatBind >= 0)
	{		
		glUniform1f(floatBind, (GLfloat)input);	
		
		success = true;
	}

	GL_CHECK_ERROR();
	return success;
}

bool Renderer::SetVector2Uniform(int programHandle, std::string uniformName, const Vector2 & input)
{
	bool success = false;

	//bind uniforms
	GLint floatBind = glGetUniformLocation(programHandle, uniformName.c_str());

	if (floatBind >= 0)
	{		
		glUniform2f(floatBind, (GLfloat)input.x, (GLfloat)input.y);	
		
		success = true;
	}

	GL_CHECK_ERROR();
	return success;
}

bool Renderer::SetVector3Uniform(int programHandle, std::string uniformName, const Vector3& input)
{
	bool success = false;

	//bind uniforms
	GLint vector3Bind = glGetUniformLocation(programHandle, uniformName.c_str());

	if (vector3Bind >= 0)
	{		
		glUniform3f(vector3Bind, (GLfloat)input.x, (GLfloat)input.y, (GLfloat)input.z);	
		success = true;
	}

	GL_CHECK_ERROR();

	return success;
}

bool Renderer::SetVector4Uniform(int programHandle, std::string uniformName, const Vector4& input)
{
	bool success = false;

	//bind uniforms
	GLint vector4Bind = glGetUniformLocation(programHandle, uniformName.c_str());

	if (vector4Bind >= 0)
	{		
		glUniform4f(vector4Bind, (GLfloat)input.x, (GLfloat)input.y, (GLfloat)input.z, (GLfloat)input.w);	
		success = true;
	}

	GL_CHECK_ERROR();

	return success;
}

void Renderer::BindMaterial(Material* material)
{
	SetShader(material->GetShader());

	for(int textureIndex = 0; textureIndex < (int)material->m_textures.size(); textureIndex++)
	{		
		SetTexture(*material->m_textures[textureIndex], textureIndex);

		//for now we are always using default
		if(textureIndex < (int)material->m_samplers.size())
		{
			BindSampler(material->m_samplers[textureIndex], textureIndex);
		}
		else
		{
			BindSampler(m_defaultSampler, textureIndex);
		}		
	}
	
	for(MaterialProperty* prop : material->m_properties)
	{
		
		if(prop->m_name == "SPECULAR_POWER")
		{
			SetSpecularPower(*(float*)prop->GetData());
		}

		else if(prop->m_name == "SPECULAR_AMOUNT")
		{
			SetSpecularAmount(*(float*)prop->GetData());
		}

		else 
		{
			prop->Bind(m_currentShader->GetProgram()->GetHandle());
		}
	}
}


void Renderer::BindRenderState(const RenderState& state)
{
	GL_CHECK_ERROR();

	glEnable(GL_BLEND);

	//color
	glBlendEquationSeparate(GetGLBlendOperationType(state.m_colorBlendOperation), GetGLBlendOperationType(state.m_alphaBlendOperation));
	glBlendFuncSeparate(GetGLBlendFactorType(state.m_colorSourceFactor), GetGLBlendFactorType(state.m_colorDestinationFactor), GetGLBlendFactorType(state.m_alphaSourceFactor), GetGLBlendFactorType(state.m_alphaDestinationFactor));
	GL_CHECK_ERROR();

	//enable/disable the test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GetGLDepthComparison(state.m_depthCompare));
	GL_CHECK_ERROR();

	// enable/disable write
	glDepthMask( state.m_depthWrite ? GL_TRUE : GL_FALSE ); 
	GL_CHECK_ERROR();

	//draw mode
	glPolygonMode(GetGLDrawFace(state.m_drawModeFace), GetGLDrawMode(state.m_drawMode));
	GL_CHECK_ERROR();

	//wind order
	glFrontFace(GetGLWindOrderType(state.m_windOrder));
	GL_CHECK_ERROR();

	if(state.m_cullMode != CULL_MODE_NONE)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GetGLCullModeType(state.m_cullMode));	
		GL_CHECK_ERROR();
	}
	else
	{
		glDisable(GL_CULL_FACE);
		GL_CHECK_ERROR();
	}
}

void Renderer::BindShaderProgram(ShaderProgram* program)
{
	GL_CHECK_ERROR();
	glUseProgram(program->GetHandle());
	GL_CHECK_ERROR();
}

void Renderer::SetShader(Shader* shader)
{
	if(shader == nullptr)
	{
		m_currentShader = m_defaultShader;		
	}
	else
	{
		m_currentShader = shader;
	}
	
	BindShaderProgram(m_currentShader->GetProgram());
}

ShaderProgram* Renderer::CreateOrGetShaderProgramFromPath(const std::string& shaderFilePath)
{
	std::map<std::string, ShaderProgram*>::iterator shaderProgram = m_loadedShaderPrograms.find(shaderFilePath);
	if(shaderProgram == m_loadedShaderPrograms.end())
	{
		bool success = false;
		ShaderProgram* newShaderProgramPtr = new ShaderProgram();

		if(shaderFilePath == "default")
		{
			success = newShaderProgramPtr->LoadBuiltInShaders(shaderFilePath, defaultShaderVS, defaultShaderFS);
		}
		else if(shaderFilePath == "invalid")
		{
			success = newShaderProgramPtr->LoadBuiltInShaders(shaderFilePath, invalidShaderVS, invalidShaderFS);
		}
		else
		{
			success = newShaderProgramPtr->LoadFromFiles(shaderFilePath.c_str());
		}		

		if(success == false)
		{
			//if shader didn't load or compile properly, we should load the invalid shader instead.
			shaderProgram = m_loadedShaderPrograms.find("invalid"); //FIND SHOULD ALWAYS WORK. Invalid should always be loaded.
			if(shaderProgram != m_loadedShaderPrograms.end())
			{
				newShaderProgramPtr->m_programHandle = shaderProgram->second->m_programHandle;
			}			
		}
		m_loadedShaderPrograms[shaderFilePath] = newShaderProgramPtr;

		return newShaderProgramPtr;
	}
	else
	{
		return m_loadedShaderPrograms[shaderFilePath];
	}
}

ShaderProgram* Renderer::CreateOrGetShaderProgramFromSeparatePaths(const std::string& id, const std::string& shaderFilePathVS, const std::string& shaderFilePathFS)
{
	TODO("Support inline shaders if coming from files");

	std::map<std::string, ShaderProgram*>::iterator shaderProgram = m_loadedShaderPrograms.find(id);
	if(shaderProgram == m_loadedShaderPrograms.end())
	{
		bool success = false;
		ShaderProgram* newShaderProgramPtr = new ShaderProgram();

		success = newShaderProgramPtr->LoadFromFiles(shaderFilePathVS.c_str(), shaderFilePathFS.c_str());

		if(success == false)
		{
			//if shader didn't load or compile properly, we should load the invalid shader instead.
			shaderProgram = m_loadedShaderPrograms.find("invalid"); //FIND SHOULD ALWAYS WORK. Invalid should always be loaded.
			if(shaderProgram != m_loadedShaderPrograms.end())
			{
				newShaderProgramPtr->m_programHandle = shaderProgram->second->m_programHandle;
			}			
		}
		m_loadedShaderPrograms[id] = newShaderProgramPtr;

		return newShaderProgramPtr;
	}
	else
	{
		return m_loadedShaderPrograms[id];
	}
}

void Renderer::UseShaderProgram(ShaderProgram* programPtr)
{
	if(programPtr != nullptr)
	{
		m_defaultShader->SetProgram(programPtr);
	}
	else
	{
		m_defaultShader->SetProgram(CreateOrGetShaderProgramFromPath("default"));
	}
	
}

TODO("Revisit affects, framebuffer swapping, copying textures whe we have samplers and more types");
void Renderer::ApplyEffect( ShaderProgram *program )
{
	if (m_effectTarget == nullptr) {
		m_effectTarget = m_defaultColorTarget;
		if (nullptr == m_effectScratch) {
			// some buffer, same size as our output
			m_effectScratch = Texture::CreateCompatible( *m_effectTarget );
		}
	}
	// I want to draw a full screen quad
	m_effectCamera->SetColorTarget( m_effectScratch );
	SetCamera( m_effectCamera );
	UseShaderProgram( program );
	SetTexture( *m_effectTarget );
	//set_sampler( 0, m_point_sampler );
	//set_texture2d( 1, m_depth_stencil_target );
	//set_sampler( 1, m_point_sampler );
	DrawAABB(AABB2(Vector2(-1.f, -1.f), Vector2(1.f, 1.f)), Rgba::WHITE);
	std::swap( m_effectTarget, m_effectScratch );
	// I want to use the program to do it
	// The current output is the input for this step
	// scratch will become the new output
	// (hence flipping output and scratch)
}
// After this call finishes - m_default_color_target should be 
//m_effect_target
void Renderer::FinishEffects()
{
	if (m_effectTarget == nullptr) {
		return;
	}
	if (m_effectTarget != m_defaultColorTarget) {
		// I ended on an odd apply effect
		CopyTexture( m_defaultColorTarget, m_effectTarget ); // 
		m_effectScratch = m_effectTarget;
	}
	// signifying done with effects... FOR NOW!!!
	m_effectTarget = nullptr;
}

