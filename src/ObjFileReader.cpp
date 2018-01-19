/*
 * ObjFileReader.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: mbauer
 */

#include "ObjFileReader.h"
#include "glm/vec2.hpp"
#include <stdio.h>

ObjFileReader::ObjFileReader()
{
    // TODO Auto-generated constructor stub

}

ObjFileReader::~ObjFileReader()
{
    // TODO Auto-generated destructor stub
}

std::vector<glm::vec3> ObjFileReader::getVertices()
{
    if (mLoadedFileSuccessfully)
    {
        return mVertices;
    }
    else
    {
        return std::vector<glm::vec3>();
    }
}

std::vector<glm::vec2> ObjFileReader::getUvs()
{
    if (mLoadedFileSuccessfully)
    {
        return mUvs;
    }
    else
    {
        return std::vector<glm::vec2>();
    }
}

std::vector<glm::vec3> ObjFileReader::getNormals()
{
    if (mLoadedFileSuccessfully)
    {
        return mNormals;
    }
    else
    {
        return std::vector<glm::vec3>();
    }
}

bool ObjFileReader::loadFile(std::string filename)
{
    mLoadedFileSuccessfully = false;
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> uvIndices;
    std::vector<unsigned int> normalIndices;

    std::vector < glm::vec3 > temp_vertices;
    std::vector < glm::vec2 > temp_uvs;
    std::vector < glm::vec3 > temp_normals;

    FILE * file = fopen(filename.c_str(), "r");
    if (file == NULL)
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (true)
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
        {
            break; // EOF = End Of File. Quit the loop.
        }

        // else : parse lineHeader
        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        mVertices.push_back(vertex);
    }

    for (auto& uvIndex : uvIndices)
    {
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        mUvs.push_back(uv);
    }

    for (auto& normalIndex : normalIndices)
    {
        glm::vec3 normal = temp_normals[normalIndex - 1];
        mNormals.push_back(normal);
    }

    mLoadedFileSuccessfully = true;
    return true;
}
