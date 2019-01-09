#ifndef OBJ_HANDLER_H
#define OBJ_HANDLER_H

//---------------------------------------------------------------------------------
//ObjHandler.h, ObjHandler.cpp
//
//Reads and writes vertex, normal, texture and face information from/to a .obj file.
//
//v - Vertex, specified as three floats
//vn - Vertex normal vector
//vt - Texture coordinate
//f - Face
//---------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <time.h>
#include <stdio.h>
#include <math.h>

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

#include "Model.h"
#include "vectorUtils.h"
#include "TGALoader.h"

using namespace std;

class Model;
class gpuModel;

class Indice
{
    public:
        Indice(int i1 = 0, int i2 = 0, int i3 = 0);
        ~Indice();

        //Indice& operator=(const Indice& in);
        Indice(const Indice& in);

        unsigned int* vert; //3 vertex numbers for triangles
};

class face
{
    public:
        face();
        ~face();
        face(const face &in);

        int nr;

        Indice vertIndex, normIndex, texIndex;

        float vertPos[9];   //x,y,z for 3 vertices
        float normDir[9];   //x,y,z dir for 3 vertices
        float texPos[9];    //r,s,t for 3 vertices. All t is the same for 2d textures

        void setVertValue(int index, int value);
        void setNormValue(int index, int value);
        void setTexValue(int index, int value);
};

class ObjHandler
{
	public:
		ObjHandler();
		ObjHandler(string filename);
		~ObjHandler();

		Model* loadObj(const string filename, const bool useTexture, const GLint rgbMode);

		void writeObj(string filename, Model *m);
		void drawModel(Model *m);

	private:
		int init(const string filename);

		Point setVertex(string file);
		Point setNormal(string file);
		Point setTexCoord(string file);

		face* createFace(string file, int numNormals, int numTexCoords);
		void createNormals();       //Create Normals Function

        void clearAllData();

		float* getVertices();		//Returns vertices as a float array, sets size as vertices size
		float* getTexCoords();		//Returns texture coordinates as a float array, sets size as texCoords size
		float* getNormals();		//Returns normals as a float array, sets size as normals size
		int* getIndices();		    //Returns faces as a float array, sets size as faces size

		//Class member variables
		vector<Point> vertices, normals, texCoords;
		vector<face*> faces;

		TGALoader* tga;
};

#endif
