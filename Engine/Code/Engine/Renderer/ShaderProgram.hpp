#include "Engine\Renderer\GlFunctions.hpp"
#include <string>

class ShaderProgram
{
	friend class Renderer;

   public:
	   ShaderProgram(){};
	   ShaderProgram(const ShaderProgram* copyShaderProgram);
	   ~ShaderProgram(){};

      bool LoadFromFiles( const char* root ); // load a shader from file
	  bool LoadFromFiles(const char* rootVS, const char* rootFS);
	  bool LoadBuiltInShaders(std::string const shaderName, std::string const programVS, std::string const programFS);
	  GLuint GetHandle( ) const { return m_programHandle;};
	  
   public:
      GLuint m_programHandle = 0; // OpenGL handle for this program, default 0
};

static GLuint LoadShader( char const *filename, GLenum type );
static GLuint LoadBuiltInShader(char const *shaderName, char const *src, GLenum type );
static GLuint CreateAndLinkProgram(  const char *root, GLint vs, GLint fs );
static void LogShaderError(char const *fileName, GLuint shaderId);
static void LogProgramError(char const *fileName, GLuint programId);

