#ifndef SHADER_MANAGER_CPP
#define SHADER_MANAGER_CPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>

#include "ShaderManager.h"

ShaderManager::ShaderManager(void)
{
}

ShaderManager::~ShaderManager(void)
{
}

GLuint ShaderManager::loadShaders(const string vert, const string frag, const string geom)
{
    GLuint program = -1;

	//Read shaders from file
	//----------------------
	const GLchar* vertFile = readFile(vert);
	const GLchar* fragFile = readFile(frag);
	const GLchar* geomFile;
	if (geom != "")
	{
        geomFile = readFile(geom);
	}


	if (*vertFile != 0 && *fragFile != 0 && ((geom != "" && *geomFile != 0) || geom == "")) //No error during reading
	{
		//Create shaders and compile
		//--------------------------
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);  //Fel! (??? 2013-12-20)
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint gs;
		if (geom != "")
        {
            gs = glCreateShader(GL_GEOMETRY_SHADER);
        }
		glShaderSource(vs, 1, &vertFile, NULL);
		glShaderSource(fs, 1, &fragFile, NULL);
		if (geom != "")
        {
            glShaderSource(gs, 1, &geomFile, NULL);
        }
		glCompileShader(vs);
		glCompileShader(fs);
		if (geom != "")
        {
            glCompileShader(gs);
        }

		//Attach shaders and link program
		//-------------------------------
		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		if (geom != "")
        {
            glAttachShader(program, fs);
		}
		glLinkProgram(program);

		//Shaders compiled successfully?
		//------------------------------
		GLint status;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
		    cout << "//------------" << endl;
			cout << "Vertex shader " << vert << " compiled unsuccessfully	-	0\n";
			printShaderInfoLog(vs);
		}
		glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
		    cout << "//------------" << endl;
			cout << "Fragment shader " << frag << " compiled unsuccessfully	-	0\n";
			printShaderInfoLog(fs);
		}
		glGetShaderiv(gs, GL_COMPILE_STATUS, &status);
		if(geom != "" && !status)
		{
		    cout << "//------------" << endl;
			cout << "Geometry shader " << geom << " compiled unsuccessfully	-	0\n";
			printShaderInfoLog(gs);
		}

		//Program compiled and linked successfully?
		//-----------------------------------------
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (!status)
		{
			cout << "Shader program unsuccessfully linked	-	0\n";
			printProgramInfoLog(program);
			cout << "//------------" << endl << endl;
		}
	}
	else
	{
		cout << "ShaderManager Error: Could not read shader files";
	}

	delete vertFile;
	delete fragFile;
	if (geom != "")
	{
        delete geomFile;
	}

	return program;
}


void ShaderManager::printShaderInfoLog(GLuint object)
{
	int infoLength = 0;
	char* infoLog;

	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLength);

	if (infoLength > 0)
	{
		infoLog = (char *)malloc(infoLength);
		glGetShaderInfoLog(object, infoLength, &infoLength, infoLog);
		cout << infoLog;
		free(infoLog);
	}
}

void ShaderManager::printProgramInfoLog(GLuint object)
{
	int infoLength = 0;
	char* infoLog;

	glGetProgramiv(object, GL_INFO_LOG_LENGTH, &infoLength);

	if (infoLength > 0)
	{
		infoLog = (char *)malloc(infoLength);
		glGetProgramInfoLog(object, infoLength, &infoLength, infoLog);
		cout << infoLog;
		free(infoLog);
	}
}

GLchar* ShaderManager::readFile(const string filename)
{
	ifstream file(filename.c_str());
	char* buf = NULL;
	string fileContents = "";
	string tempLine = "";

	if (file.is_open())
	{
	    while(file.good())
	    {
	        getline(file, tempLine);
            fileContents.append(tempLine);
            //cout << tempLine << endl;
            fileContents.append("\n");
	    }

	    buf = new char [fileContents.size()];
	    strcpy(buf, fileContents.c_str());
	    file.close();

	    return buf;
	}

	else
	{
        cerr << "ShaderManager Error: Could not read shader file " << filename << endl;
		return 0;
	}
}

#endif
