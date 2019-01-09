#ifndef MODEL_CPP
#define MODEL_CPP

#include "Model.h"
#include "TGALoader.h"

using namespace std;

Model::Model()
{
    //Initialize unity matrix
    modelMatrix = new Mat;
    (*modelMatrix)[0]  = 1.0f;
    (*modelMatrix)[5]  = 1.0f;
    (*modelMatrix)[10] = 1.0f;
    (*modelMatrix)[15] = 1.0f;

    selectable = true;
}

Model::~Model()
{
    if (vertices != NULL)
    {
        delete vertices;
        vertices = NULL;
    }

    if (normals != NULL)
    {
        delete normals;
        normals = NULL;
    }

    if (texCoords != NULL)
    {
        delete texCoords;
        texCoords = NULL;
    }
    delete texture;
    delete bumpTexture;
    delete modelMatrix;
}

void Model::draw()
{
    glBindVertexArray(vao);

    // Find current program.
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    // Get the attributes pos.
    GLint positionProgram = glGetAttribLocation(program, "inPosition");
    GLint normalProgram = glGetAttribLocation(program, "inNormal");
    GLint textureProgram = glGetAttribLocation(program, "inTexCoord");

    // Bind the models VBOs to the current program.
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glBufferData(GL_ARRAY_BUFFER, verticesSize*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(positionProgram, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionProgram);

    if(normalProgram != -1 && normalsSize > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glBufferData(GL_ARRAY_BUFFER, normalsSize*3*sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(normalProgram, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normalProgram);
    }

    if(textureProgram != -1 && texCoordsSize > 0)
    {
        //Upload texture
        glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
        glBufferData(GL_ARRAY_BUFFER, texCoordsSize*3*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(textureProgram, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(textureProgram);
    }

    //Upload ambient, diffuse and specular components
    glUniform1f(glGetUniformLocation(program, "kAmb"), kAmb);
    glUniform1f(glGetUniformLocation(program, "kDiff"), kDiff);
    glUniform1f(glGetUniformLocation(program, "kSpec"), kSpec);
    glUniform1f(glGetUniformLocation(program, "shininess"), shininess);

    // Draw the model and texture
    if(bumpTextureId > 0)
        glBindTexture(GL_TEXTURE_2D, bumpTextureId);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bumpTextureId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_TRIANGLES, 0, faces.size()*3);  //Draw
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Model::loadBumpMap(string filename, unsigned int rgbMode)
{
    unsigned int width, height;

    bumpTexture = TGALoader::loadImage(filename.c_str(), &width, &height);

//	glGenBuffers(1, vboTexture);

    glGenTextures(1, &bumpTextureId);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bumpTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, rgbMode, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bumpTexture);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    return 0;
}
/*
void Model::playAnimation(int animNr, float startTime, float transitionTime)
{

}

void Model::setModelMat(Mat newMat)
{
    modelMatrix = &newMat;
    return;
}

void Model::multModelMat(const Mat newMat)
{
    *modelMatrix = (*modelMatrix)*newMat;
    return;
}

Point Model::getVertex(int vertexID)
{
    Point res;

    res[0] = vertices[3*vertexID + 0];
    res[1] = vertices[3*vertexID + 1];
    res[2] = vertices[3*vertexID + 2];

    return res;
}


void Model::setVertex(int vertexID, Point newVertex)
{
    vertices[3*vertexID + 0] = newVertex[0];
    vertices[3*vertexID + 1] = newVertex[1];
    vertices[3*vertexID + 2] = newVertex[2];

    return;
}
*/

#endif
