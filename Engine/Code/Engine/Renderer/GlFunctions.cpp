#include "GlFunctions.hpp"
#include "GlFunctions.hpp"
#include "GlFunctions.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include "windows.h"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

#define GL_BIND_FUNCTION(f)      wglGetTypedProcAddress( &f, #f )

template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render contect
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( (HMODULE)gGLLibrary, name); 
	}

	return (*out != nullptr); 
}

//wgl functions
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

//gl functions
PFNGLCLEARPROC glClear = nullptr;
PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram= nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation= nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray= nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM2FPROC glUniform2f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLBLENDFUNCPROC glBlendFunc = nullptr;
PFNGLENABLEPROC glEnable = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr;
PFNGLGENSAMPLERSPROC glGenSamplers = nullptr;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERFVPROC	glSamplerParameterfv = nullptr;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = nullptr;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = nullptr;
PFNGLBINDSAMPLERPROC glBindSampler = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
PFNGLTEXSTORAGE2DPROC glTexStorage2D = nullptr;
PFNGLDISABLEPROC glDisable = nullptr;
PFNGLPIXELSTOREIPROC glPixelStorei = nullptr;
PFNGLGENTEXTURESPROC glGenTextures = nullptr;
PFNGLTEXPARAMETERIPROC glTexParameteri = nullptr;
PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
PFNGLDEPTHFUNCPROC glDepthFunc = nullptr;
PFNGLDEPTHMASKPROC glDepthMask = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;
PFNGLGETERRORPROC glGetError = nullptr;
PFNGLPOLYGONMODEPROC glPolygonMode = nullptr;
PFNGLCULLFACEPROC glCullFace = nullptr;
PFNGLFRONTFACEPROC glFrontFace = nullptr;
PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr; 
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;
PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D = nullptr;

void BindNewWGLFunctions()
{
	GL_BIND_FUNCTION( wglGetExtensionsStringARB ); 
	GL_BIND_FUNCTION( wglChoosePixelFormatARB ); 
	GL_BIND_FUNCTION( wglCreateContextAttribsARB );
}


void BindGLFunctions()
{
	GL_BIND_FUNCTION( glClear );
	GL_BIND_FUNCTION( glClearColor );
	GL_BIND_FUNCTION( glDeleteShader );
	GL_BIND_FUNCTION( glCreateShader );
	GL_BIND_FUNCTION( glShaderSource );
	GL_BIND_FUNCTION( glCompileShader );
	GL_BIND_FUNCTION( glGetShaderiv );
	GL_BIND_FUNCTION( glGetShaderInfoLog );
	GL_BIND_FUNCTION( glCreateProgram );
	GL_BIND_FUNCTION( glAttachShader );
	GL_BIND_FUNCTION( glLinkProgram );
	GL_BIND_FUNCTION( glGetProgramiv );
	GL_BIND_FUNCTION( glDeleteProgram );
	GL_BIND_FUNCTION( glDetachShader );
	GL_BIND_FUNCTION( glDetachShader );
	GL_BIND_FUNCTION( glGenBuffers );
	GL_BIND_FUNCTION( glBindBuffer );
	GL_BIND_FUNCTION(glBindBufferBase);
	GL_BIND_FUNCTION( glBufferData );
	GL_BIND_FUNCTION( glDeleteBuffers );
	GL_BIND_FUNCTION( glGenVertexArrays );
	GL_BIND_FUNCTION( glBindVertexArray );
	GL_BIND_FUNCTION( glGetAttribLocation );
	GL_BIND_FUNCTION( glEnableVertexAttribArray );
	GL_BIND_FUNCTION( glVertexAttribPointer );
	GL_BIND_FUNCTION( glUseProgram );
	GL_BIND_FUNCTION( glDrawArrays );
	GL_BIND_FUNCTION( glGetUniformLocation );
	GL_BIND_FUNCTION( glUniformMatrix4fv );
	GL_BIND_FUNCTION(glBlendFunc);
	GL_BIND_FUNCTION(glEnable);
	GL_BIND_FUNCTION(glGenSamplers);
	GL_BIND_FUNCTION(glSamplerParameteri);
	GL_BIND_FUNCTION(glSamplerParameterfv);
	GL_BIND_FUNCTION(glSamplerParameterf);
	GL_BIND_FUNCTION(glGenerateMipmap);
	GL_BIND_FUNCTION(glDeleteSamplers);
	GL_BIND_FUNCTION(glBindSampler);
	GL_BIND_FUNCTION(glActiveTexture);
	GL_BIND_FUNCTION(glBindTexture);
	GL_BIND_FUNCTION(glDisable);
	GL_BIND_FUNCTION(glPixelStorei);
	GL_BIND_FUNCTION(glGenTextures);
	GL_BIND_FUNCTION(glTexParameteri);
	GL_BIND_FUNCTION(glTexImage2D);
	GL_BIND_FUNCTION(glDrawElements);
	GL_BIND_FUNCTION(glClearDepthf);
	GL_BIND_FUNCTION(glDepthFunc);
	GL_BIND_FUNCTION(glDepthMask);
	GL_BIND_FUNCTION(glGenFramebuffers);
	GL_BIND_FUNCTION(glDeleteFramebuffers);
	GL_BIND_FUNCTION(glBindFramebuffer);
	GL_BIND_FUNCTION(glFramebufferTexture);
	GL_BIND_FUNCTION(glDrawBuffers);
	GL_BIND_FUNCTION(glFramebufferTexture);
	GL_BIND_FUNCTION(glCheckFramebufferStatus);
	GL_BIND_FUNCTION(glBlitFramebuffer);
	GL_BIND_FUNCTION(glGetError);
	GL_BIND_FUNCTION(glUniform1f);
	GL_BIND_FUNCTION(glUniform2f);
	GL_BIND_FUNCTION(glUniform3f);
	GL_BIND_FUNCTION(glUniform4f);
	GL_BIND_FUNCTION(glUniform1fv);
	GL_BIND_FUNCTION(glUniform2fv);
	GL_BIND_FUNCTION(glUniform3fv);
	GL_BIND_FUNCTION(glUniform4fv);
	GL_BIND_FUNCTION(glPolygonMode);
	GL_BIND_FUNCTION(glCullFace);
	GL_BIND_FUNCTION(glFrontFace);
	GL_BIND_FUNCTION(glBlendEquation);
	GL_BIND_FUNCTION(glBlendEquationSeparate);
	GL_BIND_FUNCTION(glBlendFuncSeparate);
	GL_BIND_FUNCTION(glDeleteTextures);
	GL_BIND_FUNCTION(glTexStorage2D);
	GL_BIND_FUNCTION(glTexSubImage2D);
	GL_BIND_FUNCTION(glViewport);
}

bool GLCheckError( char const* file, int line )
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		DebuggerPrintf( "GL ERROR [0x%04x] at [%s(%i)] %s", error, file, line, "\n" );
		return true; 
	}
#endif
	return false; 
}

//------------------------------------------------------------------------
bool GLFailed()
{
	return GL_CHECK_ERROR(); 
}

//------------------------------------------------------------------------
bool GLSucceeded()
{
	return !GLFailed();
}