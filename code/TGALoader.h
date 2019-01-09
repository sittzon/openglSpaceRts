#ifndef TGALOADER_H
#define TGALOADER_H

#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

class TGALoader
{
    public:
        //TGALoader();
        //~TGALoader();

        static unsigned char* loadImage(string filename, unsigned int* outWidth, unsigned int* outHeight);
    private:
        //unsigned char* imageData;
};

#endif
