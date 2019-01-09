#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

//---------------------------------------------------------------------------
//InputHandler.h, InputHandler.cpp
//
//Handles inputs from the user and acts accordingly
//
//---------------------------------------------------------------------------

#include <iostream>
#include <vector>

#include "VectorUtils.h"

using namespace std;

class InputHandler
{
	public:
		InputHandler();
		~InputHandler();

		void handleKeyboard(unsigned char key, int c, int r);
		void handleMouse(int button, int state, int c, int r);
		void handleMotion(int c, int r);
		void handleScroll(int dir);

        const vector<int>& getSelectedModels();

		bool mouseLeftDown, mouseMiddleDown, mouseRightDown;
		bool altDown;
		int cStart, rStart;

    private:
		vector<int> selectedModels;
};

#endif
