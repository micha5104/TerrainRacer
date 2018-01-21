/*
 * ObjFileReader.h
 *
 *  Created on: 19 Jan 2018
 *      Author: mbauer
 */

#ifndef OBJFILEREADER_H_
#define OBJFILEREADER_H_

#include "VertexObject.h"
#include <vector>
#include <string>
#include <map>

class ObjFileReader
{
public:
    ObjFileReader();
    virtual ~ObjFileReader();

    bool loadFile(std::string filename, float scaleFactor = 1.0);

    std::map<std::string, VertexObject> getObjects();

protected:
    bool mLoadedFileSuccessfully = false;

    std::map<std::string, VertexObject> mObjects;
};

#endif /* OBJFILEREADER_H_ */
