#include "InitHandler.h"
#include "VectorUtils.h"
#include <sstream>

using namespace VectorUtils;
using namespace Globals;

InitHandler::InitHandler()
{
    loadLevel("../../levels/1.ini");
}

InitHandler::~InitHandler()
{
}

void InitHandler::SetupBkg()
{
    //Load cubemap + background
    bkgModel = OH->loadObj(currentLevel["bkgobj"]);
    bkgModel->modelMatrix->makeScale(atof(currentLevel["bkgscale"].c_str()));
    bkgModel->modelMatrix->rotateX(PI*atof(currentLevel["bkgrotx"].c_str()));
    bkgModel->selectable = (currentLevel["bkgselectable"] == "false")? false : true;
}

void InitHandler::SetupModels()
{
    models.push_back(OH->loadObj(currentLevel["model1name"]));
    models.push_back(OH->loadObj(currentLevel["model2name"]));
    models.push_back(OH->loadObj(currentLevel["model3name"], true, GL_RGBA, Point(0.05, 0.6, 0.1, 200.0)));
    models.push_back(OH->loadObj(currentLevel["model4name"], true, GL_RGB, Point(0.00, 0.4, 0.9, 10.0)));

    //Load all models
    string currentModelString;
    for (int i = 0; i < models.size(); ++i)
    {
        currentModelString = static_cast<ostringstream*>( &(ostringstream() << i+1) )->str();
        cout << currentModelString << endl;
        std::map<string, string>::iterator it = currentLevel.find("model" + currentModelString + "rotx");
        if (it != currentLevel.end()) {
            models[i]->modelMatrix->rotateX(PI*atof(it->second.c_str()));
        }
        it = currentLevel.find("model" + currentModelString + "roty");
        if (it != currentLevel.end()) {
            models[i]->modelMatrix->rotateY(PI*atof(it->second.c_str()));
        }
        it = currentLevel.find("model" + currentModelString + "rotz");
        if (it != currentLevel.end()) {
            models[i]->modelMatrix->rotateZ(PI*atof(it->second.c_str()));
        }

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
//    models[1]->modelMatrix->rotateZ(PI);
//    models[1]->modelMatrix->rotateX(PI/2.0);
//    models[1]->modelMatrix->addTranslate(Point(-3.0, 0.0, 0.0));
//    models[2]->modelMatrix->addTranslate(Point(3.0, 2.0, .0));
//
//    models[2]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
//    models[1]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
//    models[0]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
//    models[3]->loadBumpMap("../../models/Asteroid_1_bump.tga", GL_RGBA);
}


void InitHandler::loadLevel(string levelFileName)
{
    ifstream file(levelFileName.c_str());
	string tempLine = "";
	string delimiter = "=";
    if (file.is_open())
    {
	    while(file.good())
        {
	        getline(file, tempLine);
	        string key = tempLine.substr(0, tempLine.find(delimiter));
	        tempLine.erase(0, tempLine.find(delimiter) + 1);
	        string value = tempLine;
	        currentLevel.insert(pair<string, string>(key, value));
        }
    }
    file.close();

}
