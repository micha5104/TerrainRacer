/*
 * JpegReader.h
 *
 *  Created on: 24 Dec 2017
 *      Author: mbauer
 */

#ifndef UTILS_JPEG_H_
#define UTILS_JPEG_H_

#include <string>

class Jpeg
{
public:
    Jpeg();
    virtual ~Jpeg();

    unsigned int width() const;
    unsigned int height() const;
    unsigned int channels() const;

    /* @brief reads the image data from the file
     * @return 0 if the operation was successful, otherwise: -1
     */
    int load(const std::string& filename);

    /* @brief access to the image data.
     * Layout is interleaved RGB(A)
     */
    unsigned char* data();

protected:
    unsigned int mWidth = 0;
    unsigned int mHeight = 0;
    unsigned int mChannels = 0;

    std::shared_ptr<unsigned char> mData;
};

#endif /* UTILS_JPEG_H_ */
