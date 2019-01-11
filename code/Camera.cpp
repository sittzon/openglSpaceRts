#ifndef CAMERA_CPP
#define CAMERA_CPP

#include "Camera.h"
#include "Globals.h"

using namespace Globals;

Camera::Camera()
{
    initPos             = Point(0.0f, 0.0f, -2.0f);
    initLookAt          = Point(0.0f, 0.0f, 0.0f);

    initAngleHorizontal = -3.1415-3.1415f*2.0f/3.0f;
    initAngleVertical   = 3.1415f/4.0f;
    initRadius          = 10.0f;

    reset();
}

Camera::~Camera()
{
}

void Camera::reset()
{
    pos     = initPos;
    lookAt  = initLookAt;

    angleHorizontal = initAngleHorizontal;
	angleVertical   = initAngleVertical;
	radius          = initRadius;

    update();
}

void Camera::update()
{
    pos[0] = lookAt[0] + radius*cos(angleHorizontal)*sin(angleVertical);
    pos[2] = lookAt[2] + radius*cos(angleVertical);
    pos[1] = lookAt[1] + radius*sin(angleHorizontal)*sin(angleVertical);
}

void Camera::rotate(GLfloat aH, GLfloat aV)
{
    angleHorizontal += aH;
    angleVertical += aV;
    update();
}

void Camera::zoom(GLfloat scaling)
{
    radius = radius*scaling;
    pos = lookAt - (lookAt - pos)*scaling;
    update();
}

void Camera::pan(GLfloat dx, GLfloat dy)
{
    Point c1 = Point(worldToViewMatrix[0], worldToViewMatrix[1], worldToViewMatrix[2]);
    Point c2 = Point(worldToViewMatrix[4], worldToViewMatrix[5], worldToViewMatrix[6]);

    pos    = pos    - c1*dx - c2*dy;
    lookAt = lookAt - c1*dx - c2*dy;
    update();
}

#endif
