#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

//---------------------------------------------------------------------------
//ShaderManager.h, ShaderManager.cpp
//
//Manages shader information, compiles and links vertex and fragment shaders.
//
//
//---------------------------------------------------------------------------

#include <string>
#include <cstdlib>

#ifdef _WIN32
    #ifndef GLEW_STATIC
    #define GLEW_STATIC
    #endif
    #include <glew.h>
    #include <freeglut.h>
#elif __APPLE__
    #include <OpenGL/gl3.h>
    #include <freeglut.h>
#else
    #include <freeglut.h>
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

using namespace std;

class ShaderManager
{
	public:
		ShaderManager(void);
		~ShaderManager(void);

		static GLuint loadShaders(const string vert, const string frag, const string geom = "");	//Returns a program handle for vertex, fragment and optional geometry shader

	private:
		static void printShaderInfoLog(GLuint object);						                //Prints shader info log
		static void printProgramInfoLog(GLuint object);					                    //Prints program info log
        static GLchar* readFile(const string filename);					                    //Returns a char pointer of a file from disk
};

#endif
