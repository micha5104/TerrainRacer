/*
 * Jpeg.cpp
 *
 *  Created on: 24 Dec 2017
 *      Author: mbauer
 */
#include "Jpeg.h"

#include <iostream>
#include <jpeglib.h> // /opt/local/include/jpeglib.h

Jpeg::Jpeg()
{
}

Jpeg::~Jpeg()
{
}

unsigned int Jpeg::width() const
{
    return mWidth;
}

unsigned int Jpeg::height() const
{
    return mHeight;
}

unsigned int Jpeg::channels() const
{
    return mChannels;
}

unsigned char* Jpeg::data()
{
    return mData.get();
}

int Jpeg::load(const std::string& filename)
{
    FILE *file = fopen(filename.c_str(), "rb");
    if (file == NULL)
    {
        return 1;
    }

    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err; //the error handler

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info); //fills info structure

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);

    jpeg_start_decompress(&info);

    mWidth = info.output_width;
    mHeight = info.output_height;
    mChannels = info.num_components; // 3 = RGB, 4 = RGBA
    unsigned long dataSize = mWidth * mHeight * mChannels;

    // read RGB(A) scanlines one at a time into jdata[]
    mData.reset(new unsigned char[dataSize]);
    unsigned char* rowptr;
    while (info.output_scanline < mHeight)
    {
        rowptr = mData.get() + info.output_scanline * mWidth * mChannels;
        jpeg_read_scanlines(&info, &rowptr, 1);
    }

    jpeg_finish_decompress(&info);

    fclose(file);

//    // this code block is only if you happen to want output in CImg format, but is illustrative
//    CImg<unsigned char> *image = new CImg<unsigned char>(w, h, 1, numChannels);
//    for (int x = 0; x < w; x++)
//    {
//        for (int y = 0; y < h; y++)
//        {
//            for (int c = 0; c < numChannels; c++)
//            {
//                *image->data(x, y, 0, c) = data[y * w * numChannels + x * numChannels + c];
//            }
//        }
//    }

    return 0;
}
