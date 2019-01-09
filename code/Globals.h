#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <vector>

#include "Camera.h"
#include "ObjHandler.h"
#include "InputHandler.h"
#include "ShaderManager.h"
#include "Model.h"
#include "TGALoader.h"
#include "OculusManager.h"

class InputHandler;

using namespace std;

#define PI 3.1415926535

namespace Globals
{
    extern Camera* cam;
    extern ObjHandler* OH;
    extern InputHandler* IH;

    extern unsigned int standardShaderProgram;

    extern vector<Model*> models;
    extern Model* bkgModel;

    extern unsigned int WINDOWSIZEX;
    extern unsigned int WINDOWSIZEY;

//    extern unsigned int FBOID;
//    extern unsigned int FBOTexID;
//    extern unsigned char* FBOTextureArray;

//    extern OculusManager* OVRManager;
}

#endif
