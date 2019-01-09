#include "InitHandler.h"
#include "VectorUtils.h"

using namespace VectorUtils;
using namespace Globals;

InitHandler::InitHandler()
{
    allModelNames.push_back("./models/MotherShip.obj");
    allModelNames.push_back("./models/Fighter.obj");
    allModelNames.push_back("./models/Asteroid_1.obj");
    allModelNames.push_back("./models/SpaceStation_1.obj");
}

InitHandler::~InitHandler()
{
    //dtor
}

void InitHandler::SetupModelAndTextures()
{
    //Load cubemap + background
    bkgModel = OH->loadObj("./models/cubeMapModel.obj");
    bkgModel->modelMatrix->makeScale(1000.0);
    bkgModel->modelMatrix->rotateX(PI/2.0);

    //Load all models
    vector<string>::iterator it = allModelNames.begin();
    for (; it != allModelNames.end(); it++)
    {
        //Load model
        models.push_back(OH->loadObj(*it));

        //Load bumpmap if it exist
        //models[2]->loadBumpMap("./models/Asteroid_1_bump.tga", GL_RGBA);


        //models.push_back(OH->loadObj("./models/Fighter.obj"));
        //models.push_back(OH->loadObj("./models/Asteroid_1.obj", true, GL_RGBA, Point(0.05, 0.6, 0.1, 200.0)));
        //models.push_back(OH->loadObj("./models/SpaceStation_1.obj", true, GL_RGB, Point(0.00, 0.4, 0.9, 10.0)));
    }

    //Set initial matrices and upload
//    buildPerspProjMat(projMatrix.m, 60.0f, (float)WINDOWSIZEX/WINDOWSIZEY, 0.1f, 10000.0f);
//    glUniformMatrix4fv(glGetUniformLocation(standardShaderProgram, "projection"), 1, GL_TRUE, projMatrix.m);

    //Initial pos for models
    models[1]->modelMatrix->rotateZ(PI);
    models[1]->modelMatrix->rotateX(PI/2.0);
    models[1]->modelMatrix->addTranslate(Point(-3.0, 0.0, 0.0));
    models[2]->modelMatrix->addTranslate(Point(3.0, 2.0, .0));

    models[2]->loadBumpMap("./models/Asteroid_1_bump.tga", GL_RGBA);
    models[1]->loadBumpMap("./models/Asteroid_1_bump.tga", GL_RGBA);
    models[0]->loadBumpMap("./models/Asteroid_1_bump.tga", GL_RGBA);
    models[3]->loadBumpMap("./models/Asteroid_1_bump.tga", GL_RGBA);

}
