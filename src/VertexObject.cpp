/*
 * VertexObject.cpp
 *
 *  Created on: 20 Jan 2018
 *      Author: mbauer
 */

#include "VertexObject.h"

VertexObject::VertexObject()
{
}

VertexObject::~VertexObject()
{
}

std::string VertexObject::getName() const
{
    return mName;
}

void VertexObject::setName(std::string name)
{
    mName = name;
}

void VertexObject::addFace(int vertexNum, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals)
{
    Face face;
    if (vertexNum == 3)
        face.type = Type::Triangle;
    else if (vertexNum == 4)
        face.type = Type::Quad;
    else
        face.type = Type::Poly;

    face.vertices = vertices;
    face.normals = normals;
    mFaces.push_back(face);
}

std::vector<VertexObject::Face> VertexObject::getFaces()
{
    return mFaces;
}

