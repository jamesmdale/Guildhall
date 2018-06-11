#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\File\File.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\File\File.hpp"


ShaderProgram::ShaderProgram(const ShaderProgram * copyShaderProgram)
{
	m_programHandle = copyShaderProgram->m_programHandle;
}

bool ShaderProgram::LoadFromFiles( const char* root )
{
	TODO("Remove file path addition...it doesn't make sense");
	std::string vsFile = root;
	vsFile += ".vs"; 

	std::string fsFile = root; 
	fsFile += ".fs"; 

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vertShader = LoadShader( vsFile.c_str(), GL_VERTEX_SHADER ); 
	GLuint fragShader = LoadShader( fsFile.c_str(), GL_FRAGMENT_SHADER ); 

	
	if(vertShader == 0 || fragShader == 0) //if we have an error, don't attempt to link
	{
		return false;
	}

	// Link the program
	// program_handle is a member GLuint. 
	
	m_programHandle = CreateAndLinkProgram(root, vertShader, fragShader ); 
	glDeleteShader( vertShader ); 
	glDeleteShader( fragShader ); 
	GL_CHECK_ERROR();

	return (m_programHandle != 0); 
};

bool ShaderProgram::LoadFromFiles( const char* rootVS, const char* rootFS)
{
	std::string vsFile = rootVS;
	vsFile += ".vs"; 

	std::string fsFile = rootFS; 
	fsFile += ".fs";  	

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vertShader = LoadShader( vsFile.c_str(), GL_VERTEX_SHADER ); 
	GLuint fragShader = LoadShader( fsFile.c_str(), GL_FRAGMENT_SHADER ); 


	if(vertShader == 0 || fragShader == 0) //if we have an error, don't attempt to link
	{
		return false;
	}

	// Link the program
	// program_handle is a member GLuint. 
	TODO("Better error handling for shaders from definitions");
	std::string filesRoot = "";
	m_programHandle = CreateAndLinkProgram(filesRoot.c_str(), vertShader, fragShader ); 
	glDeleteShader( vertShader ); 
	glDeleteShader( fragShader ); 
	GL_CHECK_ERROR();

	return (m_programHandle != 0); 
};


bool ShaderProgram::LoadBuiltInShaders(std::string const shaderName, std::string const programVS, std::string const programFS)
{
	//For each built in shader add the vertex and fragment
	GLuint vertShader = LoadBuiltInShader(shaderName.c_str(), programVS.c_str(), GL_VERTEX_SHADER);
	GLuint fragShader = LoadBuiltInShader(shaderName.c_str(), programFS.c_str(), GL_FRAGMENT_SHADER);

	//if we have an error don't attempt to link
	if(vertShader == 0 || fragShader == 0)
	{
		return false;
	}

	m_programHandle = CreateAndLinkProgram(shaderName.c_str(), vertShader, fragShader ); 

	return (m_programHandle != 0); 
}

static GLuint LoadShader( char const *filename, GLenum type )
{
	char *src = (char*)FileReadToNewBuffer(filename);
	GUARANTEE_RECOVERABLE(src != nullptr, "Source file not loaded!!");
	GL_CHECK_ERROR();

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	GUARANTEE_RECOVERABLE(shader_id != NULL, "Shader not loaded!!");
	GL_CHECK_ERROR();

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.

	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);
	GL_CHECK_ERROR();

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(filename, shader_id); // function we write
		glDeleteShader(shader_id);
		GL_CHECK_ERROR();
		shader_id = NULL;
	}

	free(src);

	return shader_id;
}

static GLuint LoadBuiltInShader(char const *shaderName, char const *src, GLenum type )
{
	// Create a shader
	GLuint shader_id = glCreateShader(type);
	GUARANTEE_RECOVERABLE(shader_id != NULL, "Shader not loaded!!");

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);
	GL_CHECK_ERROR();

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	GL_CHECK_ERROR();
	if (status == GL_FALSE) {
		LogShaderError(shaderName, shader_id); // function we write
		glDeleteShader(shader_id);
		GL_CHECK_ERROR();
		shader_id = NULL;
	}

	return shader_id;
}

static void LogShaderError(char const *fileName, GLuint shaderId)
{
   // figure out how large the buffer needs to be
   GLint length;
   glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
   GL_CHECK_ERROR();

   // make a buffer, and copy the log to it. 
   char *buffer = new char[length + 1];
   glGetShaderInfoLog(shaderId, length, &length, buffer);
   GL_CHECK_ERROR();

   // Print it out (may want to do some additional formatting)
   buffer[length] = NULL;
   std::string filepath = ReturnFullPath(fileName);  

   std::vector<std::string> errors = SplitStringOnCharacter((std::string)buffer, '\n');
   for(int index = 0; index < (int)errors.size(); index++) 
   {
	   if(errors[index] != "")
	   {
		   int lineNumberBegin = (int)errors[index].find('(');
		   int lineNumberEnd = (int)errors[index].find(')');
		   int lineNumber = std::stoi(errors[index].substr(lineNumberBegin + 1, lineNumberEnd - (lineNumberBegin + 1)));
		   std::string errorMessage = errors[index].substr(errors[index].find(": ") + 1);

		   ERROR_RECOVERABLE(errorMessage.c_str());
		   DebuggerPrintf( "%s(%d): %s\n", filepath.c_str(), lineNumber, errorMessage.c_str() ); // Use this specific format so Visual Studio users can double-click to jump to file-and-line of error
	   }	   
   }
   // free up the memory we used. 
   delete buffer;
}

static GLuint CreateAndLinkProgram( const char *root, GLint vs, GLint fs )
{
	// credate the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint programId = glCreateProgram();
	GUARANTEE_RECOVERABLE( programId != 0, "PROGRAM ID IS 0!!" );

	// Attach the shaders you want to use
	glAttachShader( programId, vs );
	glAttachShader( programId, fs );
	GL_CHECK_ERROR();

	// Link the program (create the GPU program)
	glLinkProgram( programId );
	GL_CHECK_ERROR();

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	GL_CHECK_ERROR();

	if (linkStatus == GL_FALSE) {
		LogProgramError(root, programId);
		glDeleteProgram(programId);
		GL_CHECK_ERROR();
		programId = 0;
	} 

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader( programId, vs );
	glDetachShader( programId, fs );
	GL_CHECK_ERROR();

	return programId;
}

static void LogProgramError(const char *fileName, GLuint programId)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
	GL_CHECK_ERROR();

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(programId, length, &length, buffer);
	GL_CHECK_ERROR();

	// print it to the output pane
	buffer[length] = NULL;
	std::string filepath = ReturnFullPath(fileName);  

	std::vector<std::string> errors = SplitStringOnCharacter((std::string)buffer, '\n');
	for(int index = 0; index < errors.size(); index++) 
	{
		if(errors[index] != "")
		{
			int lineNumberBegin = (int)errors[index].find('(');
			int lineNumberEnd = (int)errors[index].find(')');
			int lineNumber = std::stoi(errors[index].substr(lineNumberBegin + 1, lineNumberEnd - (lineNumberBegin + 1)));
			std::string errorMessage = errors[index].substr(errors[index].find(": ") + 1);

			DebuggerPrintf( "%s(%d): %s\n", filepath.c_str(), lineNumber, errorMessage.c_str() ); // Use this specific format so Visual Studio users can double-click to jump to file-and-line of error

			ERROR_RECOVERABLE(errorMessage.c_str());   
		}	   
	}	      

	// cleanup
	delete buffer;
}


