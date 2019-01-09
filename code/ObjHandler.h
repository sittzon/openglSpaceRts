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

#include "VectorUtils.h"
#include "Model.h"

class Model;

class ObjHandler
{
	public:
		ObjHandler();
		~ObjHandler();

        Model* loadObj(const std::string filename, const bool useTexture = true, const GLint rgbMode = GL_RGBA);
		Model* loadObj(const std::string filename, const bool useTexture, const GLint rgbMode, Point ads);

	private:
		int init(const std::string filename);

		Point readVertex(std::string file);
		Point readNormal(std::string file);
		Point readTexCoord(std::string file);

		void createFace(std::string file, int numNormals, int numTexCoords); //Can read quads or triangles
		void createNormals();       //Create Normals in case of no supplied

        void clearAllData();

		//Class member variables
		std::vector<Point> vertices, normals, texCoords;
		std::vector<Face*> faces;
};

#endif
