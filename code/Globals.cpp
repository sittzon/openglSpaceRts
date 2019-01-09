#ifndef GLOBALS_CPP
#define GLOBALS_CPP

#include "Globals.h"

namespace Globals
{
    using namespace std;

    Camera* cam         =  new Camera();
    ObjHandler* OH      = new ObjHandler();
    InputHandler* IH    = new InputHandler();

    unsigned int standardShaderProgram = 0;

    vector<Model*> models;
    Model* bkgModel = new Model();

    unsigned int WINDOWSIZEX = 0;
    unsigned int WINDOWSIZEY = 0;

//    unsigned int FBOID = 0;
//    unsigned int FBOTexID = 0;
//    unsigned char* FBOTextureArray;
//
//    OculusManager* OVRManager = new OculusManager();
}

#endif
