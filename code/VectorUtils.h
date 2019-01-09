#ifndef VECTORUTILS_H_INCLUDED
#define VECTORUTILS_H_INCLUDED

#include <iostream>
#include <math.h>
#include "Model.h"

class Model;

//using namespace std;

//Misc functions
//--------------
namespace VectorUtils
{
    void buildPerspProjMat(float *m, float fov, float aspect, float znear, float zfar);
    void cameraLookAt(float px, float py, float pz,
                float lx, float ly, float lz,
                float vx, float vy, float vz,
                float *m);
    int checkMouseClickOnModel(int x, int y, Mat projMat, Mat worldMat, unsigned int winX, unsigned int winY, std::vector<Model*> mdls, std::vector<int>* selMdls);
    //void loadBG(string inString);
    void drawIcon(std::string textureFileName, int rgbMode, Point inPoint);
    int createFBO(unsigned int w, unsigned int h, unsigned int rgbMode, unsigned char* textureArray, unsigned int* fboId, unsigned int* texId);
}
//--------------

class Point;

class Face
{
    public:
        float vertPos[9];   //x,y,z for 3 vertices
        float normDir[9];   //x,y,z dir for 3 vertices
        float texPos[9];    //r,s,t for 3 vertices. All t is the same for 2d textures
};

class Mat
{
    public:
        Mat();
        Mat(const Mat &m2);
        ~Mat();

        Mat operator*(const Mat &m2);                               // This * 4x4 mat m2.
        void operator*=(const Mat &m2);                             // This = This * 4x4 mat m2.
        Point operator*(const Point &pt);                           // This * homogenous vector pt.
        float& operator[](int n) const;                             // [n] take reference to element n.

        Mat operator+(const Mat &m2);                               // This + m2
        Mat operator-(const Mat &m2);                               // This - m2

        void operator=(const Mat &m2);                              // Copy constructor.

        void makeEye();                                             //Construct an identity matrix

        void makeAlignementMatrix(const Point *pta, const Point *ptb); // Make this a orthonormal matrix, M, such that M*pta = ptb.

        void addTranslate(const Point pt);     //Add translation to matrix with pt
        void setTranslate(const Point pt);     //Set translation part of matrix to pt
        void makeTranslate(const Point pt);    //Make this a translation matrix to pt

        void addScale(const float alpha);       //Multiply scaling alpha onto matrix.
        void makeScale(const float alpha);      //Make this a scale matrix of alpha.

        void makeRotationX(const float angle);
        void rotateX(const float angle);

        void makeRotationY(const float angle);
        void rotateY(const float angle);

        void makeRotationZ(const float angle);
        void rotateZ(const float angle);


        Mat transpose();    //Returns a transpose of this matrix.

        friend std::ostream& operator<<(std::ostream& o, const Mat& mat);       // For printing a matrix.

        void switchRows(int r1, int r2);           //Switches the rows r1 and r2.
        Point gaussElimination(const Point &b);    // Returns x that solves A*x = b, when A and b are given.

        float* m;
};

class Point
{
    public:
        Point();
        ~Point();
        Point(const Point &pt);
        Point& operator=(const Point &p2);

        Point(int n);
        Point(float x, float y);
        Point(float x, float y, float z);
        Point(float x, float y, float z, float h);

        friend std::ostream& operator<<(std::ostream& o, const Point& pt);        // For printing points.
        float& operator[](int n) const;                           // [] take element n.
        Point operator+(const Point a);                     // This + vector a.
        Point operator+(float a);                           // This + scalar a.
        Point operator-(const Point a);                     // This - vector a.
        Point operator-(float a);                           // This - scalar a.
        Point operator*(const Point a);                     // This .* vector a.
        Point operator*(float a);                           // This * scalar a.
        float dotProduct(const Point a);                    // This dot a.
        Point crossProduct(const Point a, const Point b);   // a cross b.
        Point crossProduct(const Point a);                  // This cross a.
        Point normalize(const Point a);                     // Normalize a.     (if numel != 4, length will be 1.0f if numel == 4, homogenous normalization.)
        void normalize();                                   // Normalize this.  (if numel != 4, length will be 1.0f if numel == 4, homogenous normalization.)
        Point negate(const Point a);                              // Return negative of a (change a to -a).
        void negate();                                      // Negate this.
        void switchElements(int e1, int e2);

        Mat matMult(const Point &a);                        // This * a' -> Mat(numel x numel);

        int numel;          //Number of elements, 4 for homogenous.

    public:
        bool drawing;
        float* coord;       //x,y,z coordinates or u,v for texture coordinates
};

#endif // VECTORUTILS_H_INCLUDED
