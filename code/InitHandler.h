#ifndef INITHANDLER_H
#define INITHANDLER_H

#include "Globals.h"
#include <map>


class InitHandler
{
    public:
        InitHandler();
        virtual ~InitHandler();


        void SetupBkg();
        void SetupModels();

        //Vector containing all model names to load
        vector<string> allModelNames;
        //Vector containing all model initial positions
        vector<float> allModelInitPos;

        void loadLevel(string levelFileName);

    protected:
    private:
        std::map<string, string> currentLevel;
};

#endif // INITHANDLER_H
