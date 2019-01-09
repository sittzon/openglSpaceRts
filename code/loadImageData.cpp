
#include "loadImageData.h"

#include <iostream>
#include <fstream>

using namespace std;

void loadImageData(int datasize, const char* fileName, char* imageData)
{

    // Assumes bmp!

    fstream fileData;
    fileData.open(fileName, fstream::in | fstream::binary);

    // Remove header

    // Remove first header (always 14 bits.)
    for(int i = 0; i < 14; ++i)
    {
        (void)fileData.get();
    }


    unsigned int headerLength = 0;

    int intTmp = 0;

    // Get DIB header size (least sigificant first)
    intTmp = fileData.get();
    headerLength = headerLength | intTmp;

    for(int i = 1; i < 4; ++i)
    {
        intTmp = fileData.get();
        intTmp = intTmp << 8*i;
        headerLength = headerLength | intTmp;
    }

    // Remove DIB header (4 bytes has been discarde when readin the size.)
    for(unsigned int i = 4; i < headerLength ; ++i)
    {
        (void)fileData.get();
    }

    // Read the pixel values.
    for(int i = 0 ; i < datasize ; ++i)
    {
      imageData[i] = fileData.get();
    }

    fileData.close();

}
