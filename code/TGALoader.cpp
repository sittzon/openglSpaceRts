#ifndef TGALOADER_CPP
#define TGALOADER_CPP

#include "TGALoader.h"
/*
TGALoader::TGALoader()
{

}
TGALoader::~TGALoader()
{
    //delete[] imageData;
}
*/
unsigned char* TGALoader::loadImage(string filename, unsigned int* outWidth, unsigned int* outHeight)
{
    FILE *file;
    unsigned char type[4];
    unsigned char info[6];

    file = fopen(filename.c_str(), "rb");

    if (!file)
        return (unsigned char*)false;

    fread (&type, sizeof(char), 3, file);
    fseek (file, 12, SEEK_SET);
    fread (&info, sizeof(char), 6, file);

    //Image type either 2 (color) or 3 (greyscale)
    if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
    {
        fclose(file);
        return (unsigned char*)false;
    }

    unsigned int width = info[0] + info[1] * 256;
    unsigned int height = info[2] + info[3] * 256;
    unsigned char byteCount = info[4] / 8;

    if (byteCount != 3 && byteCount != 4) {
        fclose(file);
        return (unsigned char*)false;
    }

    long imageSize = width * height * byteCount;

    //Allocate memory for image data
    static unsigned char* imageData = new unsigned char[imageSize];

    //read in image data
    fread(imageData, sizeof(unsigned char), imageSize, file);
    *outWidth = width;
    *outHeight = height;

//    cout << "Loaded TGA file " << filename << endl;
//    cout << "File size " << width*height*byteCount/1024 << " kB" << endl;

    //Close file
    fclose(file);

    return imageData;
}

#endif
