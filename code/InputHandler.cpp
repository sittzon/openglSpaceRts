#ifndef INPUT_HANDLER_CPP
#define INPUT_HANDLER_CPP

#include "InputHandler.h"

#include "Globals.h"

using namespace std;
using namespace Globals;

InputHandler::InputHandler()
{
	mouseLeftDown = false;
	mouseMiddleDown = false;
	cStart = 0;
	rStart = 0;
}

InputHandler::~InputHandler()
{

}

void InputHandler::handleKeyboard(unsigned char key, int, int)
{
	//int specialKey = glutGetModifiers();

    //Reset
    if(key == 'r')
    {
        //standardShaderProgram = SM->loadShaders("./code/standardPhongShader.vert", "./code/standardPhongShader.frag");
        //glUseProgram(standardShaderProgram);
        cam->reset();
    }
    //Set focus point to selected model
    else if(key == 'f' && selectedModels.size() > 0)
    {
        Model* tempModel;
        if (selectedModels.size() == 1)
        {
            tempModel = models[selectedModels[0]];
            cam->lookAt = Point(tempModel->modelMatrix->m[3], tempModel->modelMatrix->m[7], tempModel->modelMatrix->m[11]);
            cam->update();
        }
        else
        {
            //Calculate mean point
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;
            for(unsigned int i = 0; i < selectedModels.size(); i++)
            {
                tempModel = models[selectedModels[i]];
                x += tempModel->modelMatrix->m[3];
                y += tempModel->modelMatrix->m[7];
                z += tempModel->modelMatrix->m[11];
            }
            x /= selectedModels.size();
            y /= selectedModels.size();
            z /= selectedModels.size();

            //Set lookat
            cam->lookAt = Point(x, y, z);
            cam->update();
        }

        tempModel = NULL;
        delete tempModel;
    }

    else
        cout << "Key " << key << " not bound" << endl;
}

void InputHandler::handleMouse(int button, int state, int c, int r)
{
	//int specialKey = glutGetModifiers();
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// && (specialKey & GLUT_ACTIVE_ALT || specialKey & 0x0006))
    {
        mouseLeftDown = true;
        mouseMiddleDown = false;
        mouseRightDown = false;
        cStart = c;
        rStart = r;

        //Check if click is on some model
        selectedModels.clear();
        int selected = VectorUtils::checkMouseClickOnModel(c, r, cam->projectionMatrix, cam->worldToViewMatrix, WINDOWSIZEX, WINDOWSIZEY, models, &selectedModels, cam->pos);

        //If no model was selected
        if (selected == -1)
            return;

        //Is the model clicked on already selected previously?
        bool foundInList = false;
        for(unsigned int j = 0; j < selectedModels.size(); j++)
        {
            if(selectedModels[j] == selected)
            {
                foundInList = true;
                return;
            }
        }
        //If not selected previously, add to list
        if(foundInList == false)
            selectedModels.push_back(selected);
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)// && (specialKey & GLUT_ACTIVE_ALT || specialKey & 0x0006))
    {
        mouseLeftDown = false;
        mouseMiddleDown = true;
        mouseRightDown = false;
        cStart = c;
        rStart = r;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)// && (specialKey & GLUT_ACTIVE_ALT || specialKey & 0x0006))
    {
        mouseLeftDown = false;
        mouseMiddleDown = false;
        mouseRightDown = true;
        cStart = c;
        rStart = r;
    }
    else if (button == 3 && state == GLUT_DOWN) //Scroll up
    {
        cam->zoom(0.8f);
    }
    else if (button == 4 && state == GLUT_DOWN) //Scroll down
    {
        cam->zoom(1.2f);
    }
    else if (state == GLUT_UP)
    {
        mouseLeftDown = false;
        mouseMiddleDown = false;
        mouseRightDown = false;
    }
}

void InputHandler::handleMotion(int c, int r)
{
//    int selected = checkMouseClickOnModel(c, r, projMatrix, worldToView, WINDOWSIZEX, WINDOWSIZEY, models, &selectedModels);
//    if(selected != -1)
//        cout << "Selected model " << selected << endl;

    if (mouseLeftDown == true)
	{
//        cout << "Mouse left down and moving" << endl;
	}
    else if (mouseMiddleDown == true)
	{
        GLfloat dx = -(cStart-c)*cam->radius*0.0025f;
        GLfloat dy = (rStart-r)*cam->radius*0.0025f;

        cam->pan(dx, dy);

		cStart = c;
        rStart = r;
	}
	else if (mouseRightDown == true)
	{
	    GLfloat angleHorizontal = (cStart - c) * 0.005f;
	    GLfloat angleVertical = (rStart - r) * 0.005f;

        cStart = c;
        rStart = r;

        cam->rotate(angleHorizontal, angleVertical);
	}

}

void InputHandler::handleScroll(int dir)
{
    if (dir < 0)
        cam->zoom(1.2f);
    else
        cam->zoom(0.8f);
}

 const vector<int>& InputHandler::getSelectedModels()
 {
     return selectedModels;
 }


#endif
