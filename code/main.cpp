#ifndef _MAIN_CPP
#define _MAIN_CPP

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

#include <iostream>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Globals.h"
#include "InitHandler.h"

using namespace std;
using namespace VectorUtils;
using namespace Globals;

int randomNumber[100];

void init()
{
	//Display opengl version information
	cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "Max GLSL version supported: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl << endl;
	//cout << "OpenGL extensions: " << glGetString(GL_EXTENSIONS) << endl;

    //Load shaders
    standardShaderProgram = ShaderManager::loadShaders("../../code/standardPhongShader.vert", "../../code/standardPhongShader.frag");
    glUseProgram(standardShaderProgram);

    //Create FBO
//    FBOTextureArray = new unsigned char[WINDOWSIZEX*WINDOWSIZEY*4];
//    createFBO(WINDOWSIZEX, WINDOWSIZEY, GL_RGBA, FBOTextureArray, &FBOID, &FBOTexID);

    InitHandler initHandler;
    initHandler.SetupBkg();
    initHandler.SetupModels();

    //Set initial matrices and upload
    buildPerspProjMat(cam->projectionMatrix.m, 60.0f, (float)WINDOWSIZEX/WINDOWSIZEY, 0.1f, 10000.0f);
    glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "projection"), 1, GL_TRUE, cam->projectionMatrix.m);

    //Initial pos for models
    models[2]->modelMatrix->addTranslate(Point(3.0, 2.0, .0));
    models[0]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
    models[1]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
    models[2]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
    models[3]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);

    //Set camera position
    cam->update();

    //Set GL viewport
    glViewport(0, 0, WINDOWSIZEX, WINDOWSIZEY);

	//GL Inits
	glClearColor(0.0, 0.05, 0.05, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);

    //Generate random numbers for the asteroids
    srand (time(NULL));
    for(unsigned int n = 0; n < 100; n++)
    {
        randomNumber[n] = rand() % 100;
    }

    //Load OVR
    //initOVR();
}

void drawBufferObjects()
{
    //Draw all models
    for(unsigned int i = 0; i < models.size(); i++)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Draw "Select Object"
        for(unsigned int n = 0; n < IH->getSelectedModels().size(); n++)
        {
            if(IH->getSelectedModels()[n] == (int)i) //If model is selected
            {
                Point move(models[i]->modelMatrix->m[3], models[i]->modelMatrix->m[7], models[i]->modelMatrix->m[11]);
                drawIcon("../../img/target.tga", GL_RGBA, move);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            }
        }

        //Change and upload modelMatrix

        //[1] -> Fighter ship
        static float alpha = 0.0;
        models[1]->modelMatrix->makeRotationZ(PI);
        models[1]->modelMatrix->rotateX(PI/2.0);
        models[1]->modelMatrix->rotateY(sin(alpha)*PI/20.0);
        models[1]->modelMatrix->addTranslate(Point(-3.0, 0.0, 0.0));

        if(i == 0)
        {

            //[2] -> Asteroid
            models[2]->modelMatrix->rotateZ(PI/1080);
            models[2]->modelMatrix->rotateY(PI/720);
            for(unsigned int n = 0; n < 95; n++)
            {
//                models[2]->modelMatrix->makeScale(randomNumber[randomNumber[n]]/10);
                models[2]->modelMatrix->setTranslate(Point(randomNumber[n], randomNumber[n+1], randomNumber[n+2]) + Point(-50.0, -50.0, -50.0));
                glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "modelToWorld"), 1, GL_TRUE, models[2]->modelMatrix->m);
                models[2]->draw();
            }

            //[3] -> Spacestation
            models[3]->modelMatrix->makeScale(0.5);
            models[3]->modelMatrix->rotateY(sin(alpha)/5.0);
            models[3]->modelMatrix->rotateX(PI/1.5);
            models[3]->modelMatrix->rotateY(-PI/3.0);
            models[3]->modelMatrix->addTranslate(Point(5.0, -7.0, 0.0));

            alpha += 0.002f;
        }
        glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "modelToWorld"), 1, GL_TRUE, models[i]->modelMatrix->m);

        //Draw
        models[i]->draw();
    }

    //GL error check
    GLuint glErr = glGetError();
    if(glErr != 0)
        cout << "GL error main display 2: " << glErr << endl;

    //Swap buffers - redraw
    glutSwapBuffers();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //GL error check
    GLuint glErr = glGetError();
    if(glErr != 0)
        cout << "GL error main display 1: " << glErr << endl;

    cameraLookAt(cam->pos[0],cam->pos[1],cam->pos[2],
                                cam->lookAt[0],cam->lookAt[1],cam->lookAt[2],
                                0.0f,0.0f,1.0f, cam->worldToViewMatrix.m);

    //Uplod matrices
    glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "worldToView"), 1, GL_TRUE, cam->worldToViewMatrix.m);
    glUniform3f(glGetUniformLocation(standardShaderProgram, "cameraPos"), cam->pos[0], cam->pos[1], cam->pos[2]);

    //Draw bkg cubemap
    glUniform1i(glGetUniformLocation(standardShaderProgram, "usePhong"), 0);
    bkgModel->modelMatrix->setTranslate(Point(cam->pos[0], cam->pos[1], cam->pos[2]));
    glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "modelToWorld"), 1, GL_TRUE, bkgModel->modelMatrix->m);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    bkgModel->draw();
    glUniform1i(glGetUniformLocation(standardShaderProgram, "usePhong"), 1);

    //Draw to FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, FBOID);
//    drawBufferObjects();

    //Draw to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    drawBufferObjects();
}

void reshape(int w, int h)
{
    WINDOWSIZEX = w;
    WINDOWSIZEY = h;

    glViewport(0, 0, w, h);

    buildPerspProjMat(cam->projectionMatrix.m, 60.0f, (float)WINDOWSIZEX/WINDOWSIZEY, 0.1f, 10000.0f);
    glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "projection"), 1, GL_TRUE, cam->projectionMatrix.m);

    cout << "Reshape: w=" << w << " h=" << h << endl;

    //glutPostRedisplay();
    glutSwapBuffers();
}

void timerFunc(int i)
{
    //ms to wait -> fps, timerFunc, i
	glutTimerFunc(20, timerFunc, i);
	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y)
{
	IH->handleMouse(button, state, x, y);
}

void motionFunc(int x, int y)
{
	IH->handleMotion(x, y);
}

void keyboard(unsigned char key, int x, int y)
{
    IH->handleKeyboard(key, x, y);
}

void mouseWheel(int, int dir, int, int)
{
	IH->handleScroll(dir);
}

void glInit(int &argc, char **argv, int xwidth, int ywidth)
{
    //Init Window and Misc
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(700,200);
    glutInitWindowSize(xwidth, ywidth);
    glutCreateWindow("Space RTS");

    WINDOWSIZEX = xwidth;
    WINDOWSIZEY = ywidth;

    //Establish Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timerFunc, 0);

    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutKeyboardFunc(keyboard);
    glutMouseWheelFunc(mouseWheel);
}

int main(int argc, char **argv)
{
    glInit(argc, argv, 1680/2, 1050/2);

    #ifdef GLEW_STATIC
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        //Problem: glewInit failed, something is seriously wrong.
        fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    #endif

    init();
    //atexit(deleteGlobals);
    glutMainLoop();

    cout << "Process exited normally" << endl;

    return 0;
}

#endif
