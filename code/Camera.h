#ifndef CAMERA_H
#define CAMERA_H

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

#include "VectorUtils.h"

using namespace std;

class Camera
{
	public:
		Camera();
		~Camera();

		void reset();
        void update();
		void rotate(GLfloat aH, GLfloat aV);
        void zoom(GLfloat scaling);
        void pan(GLfloat dx, GLfloat dy);

        Point pos;
        Point lookAt;

        float angleHorizontal;
        float angleVertical;
        float radius;

        Mat projectionMatrix, worldToViewMatrix;

    private:
        Point initPos;      //Camera start position
        Point initLookAt;   //Camera start focus point.

        float initAngleHorizontal;
        float initAngleVertical;
        float initRadius;
};

#endif
