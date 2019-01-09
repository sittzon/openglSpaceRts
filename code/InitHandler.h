#ifndef INITHANDLER_H
#define INITHANDLER_H

#include "Globals.h"


class InitHandler
{
    public:
        InitHandler();
        virtual ~InitHandler();

        //Setup all models and textures
        void SetupModelAndTextures();

        //Vector containing all model names to load
        vector<string> allModelNames;
        //Vector containing all model initial positions
        vector<float> allModelInitPos;

    protected:
    private:
};

#endif // INITHANDLER_H
