/*
 * ObjFileReader.h
 *
 *  Created on: 19 Jan 2018
 *      Author: mbauer
 */

#ifndef OBJFILEREADER_H_
#define OBJFILEREADER_H_

#include "glm/vec3.hpp"
#include <vector>
#include <string>

class ObjFileReader
{
public:
    ObjFileReader();
    virtual ~ObjFileReader();

    bool loadFile(std::string filename);

    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getUvs();
    std::vector<glm::vec3> getNormals();

protected:
    bool mLoadedFileSuccessfully = false;
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec2> mUvs;
    std::vector<glm::vec3> mNormals;
};

#endif /* OBJFILEREADER_H_ */
