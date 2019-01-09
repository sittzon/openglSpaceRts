#ifndef VECTORUTILS_CPP
#define VECTORUTILS_CPP

#include <math.h>
#include <iostream>
#include <iomanip>

#include "VectorUtils.h"
#include "Globals.h"
#include "TGALoader.h"

using namespace Globals;

using namespace std;

namespace VectorUtils
{
    //Generate projection/camera matrix
    void buildPerspProjMat(float *m, float fov, float aspect, float znear, float zfar)
    {
      float xymax = znear * tan(fov * PI/360.0f);
      float ymin = -xymax;
      float xmin = -xymax;

      float width = xymax - xmin;
      float height = xymax - ymin;

      float depth = zfar - znear;
      float C = -(zfar + znear) / depth;
      float D = -2 * (zfar * znear) / depth;

      float w = 2 * znear / width;
      w = w / aspect;
      float h = 2 * znear / height;

      m[0]  = w; m[1]  = 0; m[2]  = 0;  m[3]  = 0;
      m[4]  = 0; m[5]  = h; m[6]  = 0; m[7]  = 0;
      m[8]  = 0; m[9]  = 0; m[10] = C; m[11] = D;
      m[12] = 0; m[13] = 0; m[14] = -1; m[15] = 0;
    }

    //Generate 4x4 lookAt matrix
    void cameraLookAt(float px, float py, float pz,
        float lx, float ly, float lz,
        float vx, float vy, float vz,
        float *m)
    {
        //Create n = p - l, normal vector
        float nx = px - lx;
        float ny = py - ly;
        float nz = pz - lz;
        //Normalize n
        float divide = sqrt(nx*nx + ny*ny + nz*nz);
        nx = nx/divide;
        ny = ny/divide;
        nz = nz/divide;

        //Create u = crossproduct(v,n), right vector
        float ux = vy*nz - vz*ny;
        float uy = vz*nx - vx*nz;
        float uz = vx*ny - vy*nx;
        //Normalize u
        divide = sqrt(ux*ux + uy*uy + uz*uz);
        ux = ux/divide;
        uy = uy/divide;
        uz = uz/divide;

        //Create v = crossproduct(n,u), up vector
        vx = ny*uz - nz*uy;
        vy = nz*ux - nx*uz;
        vz = nx*uy - ny*ux;

        //Create u*p
        float up = ux*px + uy*py + uz*pz;
        //Create v*p
        float vp = vx*px + vy*py + vz*pz;
        //Create n*p
        float np = nx*px + ny*py + nz*pz;

        m[0] = ux; m[1] = uy; m[2] = uz; m[3] = -up;
        m[4] = vx; m[5] = vy; m[6] = vz; m[7] = -vp;
        m[8] = nx; m[9] = ny; m[10] = nz; m[11] = -np;
        m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
    }

    int checkMouseClickOnModel(int x, int y, Mat projMat, Mat worldMat, unsigned int winX, unsigned int winY, vector<Model*> mdls, vector<int>* selMdls)
    {
        bool found = false;
        Face* currentFace;
        //For all models
        //--------------
        for(unsigned int i = 0; i < mdls.size(); i++)
        {
            //If model is not supposed to be selectable
            if (mdls[i]->selectable == false)
                continue;

            Mat currentModelMatrix = *(mdls[i]->modelMatrix);
            Mat resultMatrix = projMat*worldMat*currentModelMatrix;

            //For all faces
            //-------------
            vector<Face*>::iterator it = mdls[i]->faces.begin();
            for (; it != mdls[i]->faces.end(); it++)
            //for(unsigned int n = 0; n < mdls[i]->faces.size(); n++)
            {
                //Extract current vertices
                //------------------------
                //currentFace = mdls[i]->faces[n];
                currentFace = *it;
                float* modelVertices = currentFace->vertPos;

                Point vert1(modelVertices[0], modelVertices[1], modelVertices[2]);    //x,y,z position
                vert1[3] = 1.0;                               //Add h = 1.0, to be able to project
                vert1.numel = 4;
                Point vert2(modelVertices[3], modelVertices[4], modelVertices[5]);
                vert2[3] = 1.0;
                vert2.numel = 4;
                Point vert3(modelVertices[6], modelVertices[7], modelVertices[8]);
                vert3[3] = 1.0;
                vert3.numel = 4;

                //Project onto screen
                //-------------------
                Point Ah = resultMatrix*vert1;  //Projected vertex onto screen
                Ah.normalize();                 //Homogeneous normalization
                Ah.numel = 3;                   //Strip h part
                Point Bh = resultMatrix*vert2;
                Bh.normalize();
                Bh.numel = 3;
                Point Ch = resultMatrix*vert3;
                Ch.normalize();
                Ch.numel = 3;

    //            float divider = Ah[2];
    //            Ah*(1/divider);
    //            divider = Bh[2];
    //            Bh*(1/divider);
    //            divider = Ch[2];
    //            Ch*(1/divider);

                float new_x = (Ah[0] * winX ) / (2.0 * Ah[2]) + winX/2;
                float new_y = (Ah[1] * winY) / (2.0 * Ah[2]) + winY/2;
                Point A = Point(new_x, new_y);

                new_x = (Bh[0] * winX ) / (2.0 * Bh[2]) + winX/2;
                new_y = (Bh[1] * winY) / (2.0 * Bh[2]) + winY/2;
                Point B = Point(new_x, new_y);

                new_x = (Ch[0] * winX ) / (2.0 * Ch[2]) + winX/2;
                new_y = (Ch[1] * winY) / (2.0 * Ch[2]) + winY/2;
                Point C = Point(new_x, new_y);

    //            Point A = Point(Ah[0], Ah[1]);  //Normalized coordinates
    //            Point B = Point(Bh[0], Bh[1]);
    //            Point C = Point(Ch[0], Ch[1]);

    //            A[0] = winX/2*A[0] + (winX/2);    //Pixel coordinates
    //            A[1] = winY/2*A[1] + (winY/2);
    //            B[0] = winX/2*B[0] + (winX/2);
    //            B[1] = winY/2*B[1] + (winY/2);
    //            C[0] = winX/2*C[0] + (winX/2);
    //            C[1] = winY/2*C[1] + (winY/2);

    //                cout << "A: " << A << " B: " << B << " C: " << C << endl;

                float* Apixels = new float[2];
                Apixels[0] = A[0];
                Apixels[1] = A[1];
    //            glDrawPixels(1, 1, GL_RED, GL_FLOAT, Apixels);
    //            glFlush();
                delete Apixels;

                //Calculate if x,y is inside face, if true => break and return model
                //------------------------------------------------------------------
                //Compute vectors
                Point v0 = B - A;
                Point v1 = C - A;
                Point v2 = Point(x,y) - A;

                //Compute dot products
                float dot00 = v0.dotProduct(v0);
                float dot01 = v0.dotProduct(v1);
                float dot02 = v0.dotProduct(v2);
                float dot11 = v1.dotProduct(v1);
                float dot12 = v1.dotProduct(v2);

                //Compute barycentric coordinates
                float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
                float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
                float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

                //Check if point is in triangle
                if((u >= 0.0) && (v >= 0.0) && (u + v < 1.0))
                {
    //                cout << "A: " << A << " B: " << B << " C: " << C << endl;
                    cout << "Selected position " << Point(x,y) << endl;
                    found = true;
                    return i;
                }

            }
        }
        if(found == false)
        {
            selMdls->clear();
            return (-1);
        }
        return 0;
    }

    int createFBO(unsigned int w, unsigned int h, unsigned int rgbMode, unsigned char* textureArray, unsigned int* fboId, unsigned int* texId)
    {
        //Generate and bind FBO
        glGenFramebuffers(1, fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, *fboId);

        //Create and attach texture to it
        glGenTextures(1, texId);
        glBindTexture(GL_TEXTURE_2D, *texId);
        textureArray = new unsigned char[WINDOWSIZEX*WINDOWSIZEY*4];
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, rgbMode, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureArray);

        GLuint glErr = glGetError();
        if(glErr != 0)
            cout << "GL texture creation error: " << glErr << endl;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texId, 0);

        //Detach FBO and texture
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //GL error check
        glErr = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(glErr != 0)
        {
            cout << "FBO creation error: " << glErr << endl << endl;
            return 1;
        }
        else
        {
            cout << "FBO creation OK" << endl;
        }
        return 0;
    }

    //void loadBG(string inString)
    //{
    //    unsigned int width, height;
    //    TGALoader* TGALoad = new TGALoader();
    //    unsigned char* texture = TGALoad->loadImage(inString.c_str(), &width, &height);
    //    models.push_back(OH->loadObj("./models/cubeMapModel.obj"));

    //    unsigned int textureId;
    //    glGenTextures(1, &textureId);
    //
    //    //NVidia
    //    //-----------
    //    GLubyte Face[6][64][64][3];
    //    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i , GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //
    //        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
    //        0,                  //level
    //        GL_RGB8,            //internal format
    //        64,                 //width
    //        64,                 //height
    //        0,                  //border
    //        GL_RGB,             //format
    //        GL_UNSIGNED_BYTE,   //type
    //        &Face[i][0][0][0]); // pixel data
    //    }
    //
    //    //-----------
    //	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    //	glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //	glTexParameteri(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture);

    //    delete TGALoad;
    //}

    void drawIcon(string textureFileName, int rgbMode, Point inPoint)
    {
    //    static bool textureLoaded = false;
        static ObjHandler* OH = new ObjHandler();
        static Model* BB = OH->loadObj("./models/billboard.obj", false, rgbMode);

        //Load and bind texture
        unsigned int width, height;
    //    if (textureLoaded == false)
    //    {
            BB->texture = TGALoader::loadImage(textureFileName.c_str(), &width, &height);
    //        textureLoaded = true;
    //    }

        if (BB->textureId == 0)
        {
            cout << BB->textureId << endl;
            glGenTextures(1, &BB->textureId);
            cout << BB->textureId << endl;
        }
        glBindTexture(GL_TEXTURE_2D, BB->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, rgbMode, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, BB->texture);
        glBindTexture(GL_TEXTURE_2D, 0);

        //GL error check
        GLuint glErr = glGetError();
        if(glErr != 0)
            cout << "GL error drawIcon: " << glErr << endl;

        //Place in world, direction to camera
        BB->modelMatrix->makeEye();
        BB->modelMatrix->addScale(2.0);
        BB->modelMatrix->rotateZ(PI/2.0);
        BB->modelMatrix->rotateZ(cam->angleHorizontal);
        BB->modelMatrix->rotateX(cam->angleVertical);
        BB->modelMatrix->addTranslate(inPoint);
        glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "modelToWorld"), 1, GL_TRUE, BB->modelMatrix->m);

        //Draw billboard
        glUniform1i(glGetUniformLocation(standardShaderProgram, "usePhong"), 0);
        BB->draw();
        glUniform1i(glGetUniformLocation(standardShaderProgram, "usePhong"), 1);

        //Free memory
    //    OH = NULL;
    //    tga = NULL;
    //    delete OH;
    }
}

Mat::Mat()
{
    m = new float[16];
    for(int i = 0; i < 16; i++)
    {
        m[i] = 0.0f;
    }
}

Mat::Mat(const Mat &m2)
{
    m = new float[16];
    for(int i = 0; i < 16; i++)
    {
        m[i] = m2.m[i];
    }
}

Mat::~Mat()
{
    if(m != NULL)
    {
        //delete m; //Throws error- glibc detected : invalid pointer
        delete[] m;
        m = NULL;
    }
}

Mat Mat::operator*(const Mat &m2)
{
    Mat* out = new Mat();
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            out->m[4*r + c] =   m[4*r]   * m2.m[c]   + m[4*r+1] * m2.m[c+4] +
                                m[4*r+2] * m2.m[c+8] + m[4*r+3] * m2.m[c+12];
        }
    }

	return *out;
}

void Mat::operator*=(const Mat &m2)
{
    Mat newMat;// = Mat();
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            newMat[4*r + c] =   m[4*r]  *  m2.m[c]   + m[4*r+1] * m2.m[c+4] +
                                m[4*r+2] * m2.m[c+8] + m[4*r+3] * m2.m[c+12];
        }
    }

    m[ 0] = newMat[ 0];    m[ 1] = newMat[ 1];    m[ 2] = newMat[ 2];    m[ 3] = newMat[ 3];
    m[ 4] = newMat[ 4];    m[ 5] = newMat[ 5];    m[ 6] = newMat[ 6];    m[ 7] = newMat[ 7];
    m[ 8] = newMat[ 8];    m[ 9] = newMat[ 9];    m[10] = newMat[10];    m[11] = newMat[11];
    m[12] = newMat[12];    m[13] = newMat[13];    m[14] = newMat[14];    m[15] = newMat[15];

    return;
}


Point Mat::operator*(const Point& pt)
{
    Point out = Point(0.0f, 0.0f, 0.0f, 0.0f);
    pt[0];

    out[0] = m[ 0]*pt[0] + m[ 1]*pt[1] + m[ 2]*pt[2] + m[ 3]*pt[3];
    out[1] = m[ 4]*pt[0] + m[ 5]*pt[1] + m[ 6]*pt[2] + m[ 7]*pt[3];
    out[2] = m[ 8]*pt[0] + m[ 9]*pt[1] + m[10]*pt[2] + m[11]*pt[3];
    out[3] = m[12]*pt[0] + m[13]*pt[1] + m[14]*pt[2] + m[15]*pt[3];

	return out;
}

/*
Point Mat::operator*(const Point *pt)
{
    Point out = Point(0.0f, 0.0f, 0.0f, 0.0f);

    out[0] = m[ 0]*pt->coord[0] + m[ 1]*pt->coord[1] + m[ 2]*pt->coord[2] + m[ 3]*pt->coord[3];
    out[1] = m[ 4]*pt->coord[0] + m[ 5]*pt->coord[1] + m[ 6]*pt->coord[2] + m[ 7]*pt->coord[3];
    out[2] = m[ 8]*pt->coord[0] + m[ 9]*pt->coord[1] + m[10]*pt->coord[2] + m[11]*pt->coord[3];
    out[3] = m[12]*pt->coord[0] + m[13]*pt->coord[1] + m[14]*pt->coord[2] + m[15]*pt->coord[3];

	return out;
}
*/
float& Mat::operator[](int n)
{
    return m[n];
}


Mat Mat::operator+(const Mat &m2)
{
    Mat newMat = Mat();
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            newMat[4*r + c] =   m[4*r + c] + m2.m[4*r + c];
        }
    }
    return newMat;
}

Mat Mat::operator-(const Mat &m2)
{
    Mat newMat = Mat();
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            newMat[4*r + c] =   m[4*r + c] - m2.m[4*r + c];
        }
    }
    return newMat;
}


void Mat::operator=(const Mat &m2)
{
    m[ 0] = m2.m[ 0]; m[ 1] = m2.m[ 1]; m[ 2] = m2.m[ 2]; m[ 3] = m2.m[ 3];
    m[ 4] = m2.m[ 4]; m[ 5] = m2.m[ 5]; m[ 6] = m2.m[ 6]; m[ 7] = m2.m[ 7];
    m[ 8] = m2.m[ 8]; m[ 9] = m2.m[ 9]; m[10] = m2.m[10]; m[11] = m2.m[11];
    m[12] = m2.m[12]; m[13] = m2.m[13]; m[14] = m2.m[14]; m[15] = m2.m[15];
    return;
}

void Mat::makeEye()
{
    m[ 0] = 1.0f; m[ 1] = 0.0f; m[ 2] = 0.0f; m[ 3] = 0;
    m[ 4] = 0.0f; m[ 5] = 1.0f; m[ 6] = 0.0f; m[ 7] = 0;
    m[ 8] = 0.0f; m[ 9] = 0.0f; m[10] = 1.0f; m[11] = 0;
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
}

void Mat::makeAlignementMatrix(const Point *pta, const Point *ptb)
{
    Point a = *pta;
    Point aT1;
    Point aT2;
    Point b = *ptb;
    Point bT1;
    Point bT2;

    a.normalize();
    b.normalize();

    Mat aMat = Mat();
    Mat bMat = Mat();

    if(fabs(a[0]) <= 0.0)
    {
        aT1 = Point(1.0, 0.0, 0.0);
    }
    else
    {
        aT1 = Point(0.0, 1.0, 0.0).crossProduct(a);
        aT1.normalize();
    }
    if(fabs(b[0]) <= 0.0)
    {
        bT1 = Point(1.0, 0.0, 0.0);
    }
    else
    {
        bT1 = Point(0.0, 1.0, 0.0).crossProduct(b);
        bT1.normalize();
    }

    aT2 = a.crossProduct(aT1);
    bT2 = b.crossProduct(bT1);

    aMat[0] = a[0]; aMat[1] = aT1[0]; aMat[2]  = aT2[0]; aMat[3]  = 0.0;
    aMat[4] = a[1]; aMat[5] = aT1[1]; aMat[6]  = aT2[1]; aMat[7]  = 0.0;
    aMat[8] = a[2]; aMat[9] = aT1[2]; aMat[10] = aT2[2]; aMat[11] = 0.0;
    aMat[12] = 0.0; aMat[13] = 0.0;   aMat[14] = 0.0;    aMat[15] = 1.0;

    bMat[0] = b[0]; bMat[1] = bT1[0]; bMat[2]  = bT2[0]; bMat[3]  = 0.0;
    bMat[4] = b[1]; bMat[5] = bT1[1]; bMat[6]  = bT2[1]; bMat[7]  = 0.0;
    bMat[8] = b[2]; bMat[9] = bT1[2]; bMat[10] = bT2[2]; bMat[11] = 0.0;
    bMat[12] = 0.0; bMat[13] = 0.0;   bMat[14] = 0.0;    bMat[15] = 1.0;

    *this = bMat;
    *this *= aMat.transpose();
}

void Mat::addTranslate(const Point pt)
{
    m[ 3] += pt.coord[0];
    m[ 7] += pt.coord[1];
    m[11] += pt.coord[2];
}

void Mat::setTranslate(const Point pt)
{
    m[ 3] = pt.coord[0];
    m[ 7] = pt.coord[1];
    m[11] = pt.coord[2];
}


void Mat::makeTranslate(const Point pt)
{
    m[ 0] = 1.0f; m[ 1] = 0.0f; m[ 2] = 0.0f; m[ 3] = pt.coord[0];
    m[ 4] = 0.0f; m[ 5] = 1.0f; m[ 6] = 0.0f; m[ 7] = pt.coord[1];
    m[ 8] = 0.0f; m[ 9] = 0.0f; m[10] = 1.0f; m[11] = pt.coord[2];
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;

    // Alt.
    //makeEye();
    //addTranslate(pt);
    return;
}

void Mat::addScale(const float alpha)
{
    m[ 0] *= alpha;    m[ 1] *= alpha;    m[ 2] *= alpha;
    m[ 4] *= alpha;    m[ 5] *= alpha;    m[ 6] *= alpha;
    m[ 8] *= alpha;    m[ 9] *= alpha;    m[10] *= alpha;
    return;
}
void Mat::makeScale(const float alpha)
{
    m[ 0] = alpha; m[ 1] = 0.0f;  m[ 2] = 0.0f;  m[ 3] = 0.0f;
    m[ 4] = 0.0f;  m[ 5] = alpha; m[ 6] = 0.0f;  m[ 7] = 0.0f;
    m[ 8] = 0.0f;  m[ 9] = 0.0f;  m[10] = alpha; m[11] = 0.0f;
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;  m[15] = 1.0f;

    //Alt.
    //makeEye();
    //addScale(alpha);
    return;
}

void Mat::makeRotationX(const float angle)
{
    m[ 0] = 1.0f;   m[ 1] = 0.0f;           m[ 2] = 0.0f;           m[ 3] = 0.0f;
    m[ 4] = 0.0f;   m[ 5] = cos(angle);     m[ 6] = -sin(angle);    m[ 7] = 0.0f;
    m[ 8] = 0.0f;   m[ 9] = sin(angle);     m[10] = cos(angle);     m[11] = 0.0f;
    m[12] = 0.0f;   m[13] = 0.0f;           m[14] = 0.0f;           m[15] = 1.0f;
}
void Mat::rotateX(const float angle)
{
    Mat temp = Mat();
    temp.makeRotationX(angle);
    *this *= temp;
}

void Mat::makeRotationY(const float angle)
{
    m[ 0] = cos(angle);     m[ 1] = 0.0f;  m[ 2] = sin(angle);      m[ 3] = 0.0f;
    m[ 4] = 0.0f;           m[ 5] = 1.0f;  m[ 6] = 0.0f;            m[ 7] = 0.0f;
    m[ 8] = -sin(angle);    m[ 9] = 0.0f;  m[10] = cos(angle);      m[11] = 0.0f;
    m[12] = 0.0f;           m[13] = 0.0f;  m[14] = 0.0f;            m[15] = 1.0f;
}
void Mat::rotateY(const float angle)
{
    Mat temp = Mat();
    temp.makeRotationY(angle);
    *this *= temp;
}

void Mat::makeRotationZ(const float angle)
{
    m[ 0] = cos(angle);     m[ 1] = -sin(angle);    m[ 2] = 0.0f;   m[ 3] = 0.0f;
    m[ 4] = sin(angle);     m[ 5] = cos(angle);     m[ 6] = 0.0f;   m[ 7] = 0.0f;
    m[ 8] = 0.0f;           m[ 9] = 0.0f;           m[10] = 1.0f;   m[11] = 0.0f;
    m[12] = 0.0f;           m[13] = 0.0f;           m[14] = 0.0f;   m[15] = 1.0f;
}
void Mat::rotateZ(const float angle)
{
    Mat temp = Mat();
    temp.makeRotationZ(angle);
    *this *= temp;
}

Mat Mat::transpose()
{
    Mat newMat = Mat();

    newMat[ 0] = m[ 0]; newMat[ 1] = m[ 4]; newMat[ 2] = m[ 8]; newMat[ 3] = m[12];
    newMat[ 4] = m[ 1]; newMat[ 5] = m[ 5]; newMat[ 6] = m[ 9]; newMat[ 7] = m[13];
    newMat[ 8] = m[ 2]; newMat[ 9] = m[ 6]; newMat[10] = m[10]; newMat[11] = m[14];
    newMat[12] = m[ 3]; newMat[13] = m[ 7]; newMat[14] = m[11]; newMat[15] = m[15];

    return newMat;
}

ostream& operator<<(ostream& o, const Mat& mat)
{
    o << endl << "[";

    for (int r = 0 ; r < 4 ; ++r)
    {
        o << endl << "  [" << setw(5) << mat.m[r*4];

        for (int c = 1 ; c < 4 ; ++c)
        {
            o << ", " << setw(5) << mat.m[r*4 + c];
        }

        o << "]";
    }

    o << endl << "]";

    return o;
}

void Mat::switchRows(int r1, int r2)
{
    float tmp1 = m[0 + 4*r1];
    float tmp2 = m[1 + 4*r1];
    float tmp3 = m[2 + 4*r1];
    float tmp4 = m[3 + 4*r1];

    m[0 + 4*r1] = m[0 + 4*r2];
    m[1 + 4*r1] = m[1 + 4*r2];
    m[2 + 4*r1] = m[2 + 4*r2];
    m[3 + 4*r1] = m[3 + 4*r2];

    m[0 + 4*r2] = tmp1;
    m[1 + 4*r2] = tmp2;
    m[2 + 4*r2] = tmp3;
    m[3 + 4*r2] = tmp4;

    return;
}

// Returns x that solves A*x = b, when A and b are given.
Point Mat::gaussElimination(const Point &b)
{
    // Multiplication factors.
    Mat mults = Mat();

    // Copy this mat to A;
    Mat A = Mat();
    A = *this;

    //Copy n to x
    Point x = Point(4);
    x = b;

    int rowOrder[4] = {0, 1, 2, 3};

    int n = 0;
    int temp = 0;
    int r1 = 0;
    int r2 = 0;


//
//    cout << endl << "Starting A matrix: " << endl;
//    cout << A << endl;
//    cout << "b vector: " << b << endl;
//    cout << "x vector: " << x << endl;

    for(int k = 0; k < 4; ++k)
    {
        //Check if element is zero
        if(m[k + 4*k] == 0.0f)
        {
            //Change row order until element i, i isn't zero.
            n = 0;
            while(A[k + 4*k] == 0.0f)
            {
                n++;
                r1 = k;
                r2 = k+n;

                if(r2 >= 4)
                {
                    cout << "Matrix is singular, no Gauss elimination could be done!" << endl;
                    cout << *this << endl;
                    cout << "Failed in row " << k << endl;
                    cout << "A mat: " << endl << A << endl;
                    break;
                }

                A.switchRows(r1, r2);
                x.switchElements(r1, r2);

                // Keep track of rowchanges.
                temp = rowOrder[r1];
                rowOrder[r1] = rowOrder[r2];
                rowOrder[r2] = temp;
            }

        }

        for(int i = k+1; i < 4; ++i) // Rows
        {
            mults[k + 4*i] = A[k + 4*i]/A[k + 4*k];
        }
//
//        cout << "Multiplications mat:" << endl;
//        cout << mults << endl;

        for(int i = k+1; i < 4; ++i) // Rows.
        {
            for(int j = k; j < 4; ++j) // Collumns
            {
                A[j + i*4] -= mults[k + 4*i]*A[j + 4*k];
            }
//            cout << " : " << x[i];
            x[i] -= mults[k + 4*i]*x[k];
//            cout << " - " << mults[k + 4*i] << " * " << x[k] <<  " = " << x[i] << endl;
        }
//
//        cout << "A matrix: " << endl;
//        cout << A << endl;
//        cout << "x vector: " << x << endl;
//        cout << "Row order:" << rowOrder[0] << ", " << rowOrder[1] << ", " << rowOrder[2] << ", " << rowOrder[3] << endl;
//        cout << endl << endl;

    }

    // Switch back elements in x.
    for(int i = 0; i < 3; ++i) // Last element is ok if the others are.
    {
        // If wrong order, switch.
        if(rowOrder[i] != i)
        {
            x.switchElements(i, i+1);
                temp = rowOrder[i];
                rowOrder[i] = rowOrder[i+1];
                rowOrder[i+1] = temp;
        }
    }

    // Here A is a right triangular matrix;

    for(int k = 3; k >= 0 ; k--)
    {

        for(int i = k + 1; i < 4 ; i++) // Collumns
        {
            x[k] -= x[i]* A[i + 4*k];
        }

        x[k] = x[k] / A[k + 4*k];
    }
//
//    cout << "Result: " << x << endl;
//
//    cout << "b = " << b << endl << "A*x = " << (*this)*x << endl;

    return x;

}

// ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Point ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Point::Point()
{
    numel = 3;
    coord = new float[3];
    drawing = false;
}

Point::Point(const Point &pt)
{
    numel = pt.numel;
    coord = new float[numel];
    drawing = false;

    for(int n = 0 ; n < numel ; ++n)
    {
        coord[n] = pt.coord[n];
    }
}

Point::Point(int n)
{
    numel = n;
    coord = new float[n];
    drawing = false;
}

Point::Point(float x, float y)
{
    numel = 2;
    coord = new float[2];
    drawing = false;
    coord[0] = x; coord[1] = y;
}

Point::Point(float x, float y, float z)
{
    numel = 3;
    coord = new float[3];
    drawing = false;
    coord[0] = x; coord[1] = y; coord[2] = z;
}

Point::Point(float x, float y, float z, float h)
{
    numel = 4;
    coord = new float[4];
    drawing = false;
    coord[0] = x; coord[1] = y; coord[2] = z; coord[3] = h;
}

Point::~Point()
{
    if(drawing)
    {
        //getGlobal()->DPH->removePoint(this);
    }

    if(coord != NULL)
    {
        delete[] coord;
        coord = NULL;
    }
}

ostream& operator<<(ostream& o, const Point& pt)
{
    o << "["  << setw(5) << pt.coord[0];

    for (int n = 1 ; n < pt.numel ; ++n)
    {
        o << ", " << setw(5) << pt.coord[n];
    }

    o << "]";

    return o;
}

Point& Point::operator=(const Point &p2)
{
    // Copy the first values from p2 change size if necessary.
    if(numel != p2.numel)
    {
        numel = p2.numel;
        delete[] coord;
        coord = new float[numel];
    }

    for(int n = 0 ; n < numel ; ++n)
    {
        coord[n] = p2.coord[n];
    }

    return *this;
}

float& Point::operator[](int n)
{
    return coord[n];
}

Point Point::operator+(const Point a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] + a.coord[0];
    out.coord[1] = coord[1] + a.coord[1];

    if(numel >=3 && a.numel >=3)
    {
        out.coord[2] = coord[2] + a.coord[2];
        if(numel == 4 && a.numel == 4)
        {
            out.coord[3] = coord[3] + a.coord[3];
        }
    }

    return out;
}
Point Point::operator+(float a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] + a;
    out.coord[1] = coord[1] + a;

    if(numel >=3)
    {
        out.coord[2] = coord[2] + a;
        if(numel == 4)
        {
            out.coord[3] = coord[3] + a;
        }
    }
    return out;
}
Point Point::operator-(const Point a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] - a.coord[0];
    out.coord[1] = coord[1] - a.coord[1];

    if(numel >=3 && a.numel >=3)
    {
        out.coord[2] = coord[2] - a.coord[2];

        if(numel == 4 && a.numel == 4)
        {
            out.coord[3] = coord[3] - a.coord[3];
        }
    }
    return out;
}
Point Point::operator-(float a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] - a;
    out.coord[1] = coord[1] - a;

    if(numel >=3)
    {
        out.coord[2] = coord[2] - a;
        if(numel == 4)
        {
            out.coord[3] = coord[3] - a;
        }
    }
    return out;
}
Point Point::operator*(const Point a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] * a.coord[0];
    out.coord[1] = coord[1] * a.coord[1];

    if(numel >=3 && a.numel >=3)
    {
        out.coord[2] = coord[2] * a.coord[2];

        if(numel == 4 && a.numel == 4)
        {
            out.coord[3] = coord[3] * a.coord[3];
        }
    }
    return out;
}
Point Point::operator*(float a)
{
    Point out = Point(numel);//Point out;
    out.coord[0] = coord[0] * a;
    out.coord[1] = coord[1] * a;

    if(numel >=3)
    {
        out.coord[2] = coord[2] * a;
        if(numel == 4)
        {
            out.coord[3] = coord[3] * a;
        }
    }
    return out;
}
float Point::dotProduct(const Point a)
{
    float out = 0.0f;

    if(numel >= 2)
    {
        out = coord[0] * a.coord[0] + coord[1] * a.coord[1];
        if(numel >= 3)
        {
            out += coord[2] * a.coord[2];
            if(numel >= 4)
            {
                out += coord[3] * a.coord[3];
            }
        }
    }

    return out;
}
/*
Point Point::crossProduct(const Point a, const Point b)
{
    Point tempPoint = Point(3);
    tempPoint.coord[0] = a.coord[1]*b.coord[2] - a.coord[2]*b.coord[1];
    tempPoint.coord[1] = a.coord[2]*b.coord[0] - a.coord[0]*b.coord[2];
    tempPoint.coord[2] = a.coord[0]*b.coord[1] - a.coord[1]*b.coord[0];
    return tempPoint;
}
*/
Point Point::crossProduct(const Point a)
{
    Point tempPoint = Point(3);
    tempPoint.coord[0] = coord[1]*a.coord[2] - coord[2]*a.coord[1];
    tempPoint.coord[1] = coord[2]*a.coord[0] - coord[0]*a.coord[2];
    tempPoint.coord[2] = coord[0]*a.coord[1] - coord[1]*a.coord[0];
    return tempPoint;
}

Point Point::normalize(Point a)
{
    float divider = 1.0f;

    if(numel == 4)
    {
        divider = a[3];
    }
    else
    {
        divider = sqrt(a.coord[0]*a.coord[0]+a.coord[1]*a.coord[1]+a.coord[2]*a.coord[2]);
    }

    a.coord[0] = a.coord[0]/divider;
    a.coord[1] = a.coord[1]/divider;
    a.coord[2] = a.coord[2]/divider;
    return a;
}

void Point::normalize()
{
    float divider = 1.0f;

    if(numel == 4)
    {
        divider = coord[3];
        coord[3] = 1.0f;
    }
    else
    {
        divider = sqrt(coord[0]*coord[0]+coord[1]*coord[1]+coord[2]*coord[2]);
    }

    coord[0] = coord[0]/divider;
    coord[1] = coord[1]/divider;
    coord[2] = coord[2]/divider;
}

/*
Point Point::negate(Point a)
{
    a.coord[0] = -a.coord[0];
    a.coord[1] = -a.coord[1];
    a.coord[2] = -a.coord[2];
    return a;
}

void Point::negate()
{
    coord[0] = -coord[0];
    coord[1] = -coord[1];
    coord[2] = -coord[2];
}
*/
 void Point::switchElements(int e1, int e2)
 {
     float temp = coord[e1];
     coord[e1] = coord[e2];
     coord[e2] = temp;

     return;
 }

Mat Point::matMult(const Point &a)
{
     Mat res;
     if(numel == 4 && a.numel == 4)
     {
        res[ 0] = coord[0] * a.coord[0]; res[ 1] = coord[0] * a.coord[1]; res[ 2] = coord[0] * a.coord[2]; res[ 3] = coord[0] * a.coord[3];
        res[ 4] = coord[1] * a.coord[0]; res[ 5] = coord[1] * a.coord[1]; res[ 6] = coord[1] * a.coord[2]; res[ 7] = coord[1] * a.coord[3];
        res[ 8] = coord[2] * a.coord[0]; res[ 9] = coord[2] * a.coord[1]; res[10] = coord[2] * a.coord[2]; res[11] = coord[2] * a.coord[3];
        res[12] = coord[3] * a.coord[0]; res[13] = coord[3] * a.coord[1]; res[14] = coord[3] * a.coord[2]; res[15] = coord[3] * a.coord[3];
     }
     else
     {
         cout << "Unable to comply... matMult needs homogenous points.";
     }

     return res;
}
/*
void Point::addToDrawnPoints()
{
    drawing = true;
    //getGlobal()->DPH->addPoint(this);
    return;
}
*/
#endif
