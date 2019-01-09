#ifndef MODEL_H
#define MODEL_H

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

#include <string>
#include <vector>

class Face;
class Mat;
class Point;

#include "VectorUtils.h"
#include "ObjHandler.h"

using namespace std;

class Model
{
    public:
        Model();
        ~Model();

        void draw();
        int loadBumpMap(string filename, unsigned int rgbMode);

//        void playAnimation(int animNr, float startTime = 0.0f, float transitionTime = 0.0f);

//        void setModelMat(Mat newMat);
//        void multModelMat(Mat newMat);

        //Point getVertex(int vertexID);
//        void setVertex(int vertexID, Point newVertex);

        float* vertices;
        float* normals;
        float* texCoords;
		vector<Face*> faces;
        string name;

        Mat* modelMatrix;
        unsigned char* texture;
        unsigned char* bumpTexture;

        unsigned int verticesSize, normalsSize, texCoordsSize;
        unsigned int vao, vboVertex, vboNormal, vboTexture;
        unsigned int textureId, bumpTextureId;

        float kAmb, kDiff, kSpec;
        float shininess;

        bool selectable;
};

#endif
