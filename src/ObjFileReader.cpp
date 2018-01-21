/*
 * ObjFileReader.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: mbauer
 */

#include "ObjFileReader.h"
#include "Utils.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>

ObjFileReader::ObjFileReader()
{
}

ObjFileReader::~ObjFileReader()
{
}

std::map<std::string, VertexObject> ObjFileReader::getObjects()
{
    if (mLoadedFileSuccessfully)
    {
        return mObjects;
    }
    else
    {
        return std::map<std::string, VertexObject>();
    }
}

bool ObjFileReader::loadFile(std::string filename, float scaleFactor)
{
    mLoadedFileSuccessfully = false;
    mObjects.clear();

    std::vector < glm::vec3 > temp_vertices;
    std::vector < glm::vec2 > temp_uvs;
    std::vector < glm::vec3 > temp_normals;

    VertexObject obj;
    bool objIsValid = false;

    std::ifstream infile(filename);
    if (infile.good() == false)
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::vector<std::string> words = Utils::split(line, ' ');

        std::string lineHeader = words[0];
        if (lineHeader == "o")
        {
            if (objIsValid)
            {
                mObjects[obj.getName()] = obj;
                obj = VertexObject();
            }
            objIsValid = true;
        }
        else if (lineHeader == "g")
        {
            obj.setName(words[1]);
        }
        else if (lineHeader == "v")
        {
            glm::vec3 vertex;
            vertex.x = std::stof(words[1]);
            vertex.y = std::stof(words[2]);
            vertex.z = std::stof(words[3]);
            temp_vertices.push_back(vertex * scaleFactor);
        }
        else if (lineHeader == "vt")
        {
            glm::vec2 uv;
            uv.x = std::stof(words[1]);
            uv.y = std::stof(words[2]);
            temp_uvs.push_back(uv);
        }
        else if (lineHeader == "vn")
        {
            glm::vec3 normal;
            normal.x = std::stof(words[1]);
            normal.y = std::stof(words[2]);
            normal.z = std::stof(words[3]);
            temp_normals.push_back(normal);
        }
        else if (lineHeader == "f")
        {
            int vertexCounter = 0;
            int matches = 0;
            std::vector<unsigned int> vertexIndices;
            std::vector<unsigned int> uvIndices;
            std::vector<unsigned int> normalIndices;

            int index = 0;
            for (int i = 1; i < words.size(); ++i)
            {
                std::vector<std::string> indices = Utils::split(words[i], '/');
                if (indices.size() > 2)
                {
                    unsigned int vertexIndex = indices[0].empty() ? 0 : std::stoi(indices[0]);
                    unsigned int uvIndex = indices[1].empty() ? 0 : std::stoi(indices[1]);
                    unsigned int normalIndex = indices[2].empty() ? 0 : std::stoi(indices[2]);

                    vertexCounter++;
                    vertexIndices.push_back(vertexIndex);
                    uvIndices.push_back(uvIndex);
                    normalIndices.push_back(normalIndex);
                }
                index++;
            }

            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            for (const auto& vertexIndex : vertexIndices)
            {
                vertices.push_back(temp_vertices[vertexIndex]);
            }
            for (const auto& normalIndex : normalIndices)
            {
                normals.push_back(temp_normals[normalIndex]);
            }

            obj.addFace(vertexCounter, vertices, normals);
        }
    }
    if (objIsValid)
    {
        mObjects[obj.getName()] = obj;
    }

    mLoadedFileSuccessfully = true;
    return true;
}
