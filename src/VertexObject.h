/*
 * VertexObject.h
 *
 *  Created on: 20 Jan 2018
 *      Author: mbauer
 */

#ifndef SRC_VERTEXOBJECT_H_
#define SRC_VERTEXOBJECT_H_

#include "glm/vec3.hpp"
#include <vector>
#include <string>

class VertexObject
{
public:
    enum class Type
    {
        Triangle,
        Quad,
        Poly
    };
    struct Face
    {
        Type type;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
    };

    VertexObject();
    virtual ~VertexObject();

    std::string getName() const;
    void setName(std::string name);
    void addFace(int vertexNum, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals);
    std::vector<Face> getFaces();

protected:
    std::vector<Face> mFaces;
    std::string mName;
};

#endif /* SRC_VERTEXOBJECT_H_ */
