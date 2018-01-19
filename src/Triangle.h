#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

#include <array>

class Triangle
{
public:
    Triangle();
    ~Triangle() = default;
    Triangle(const Triangle& other);

    void setCorner(int index, const glm::vec3& coord);
    glm::vec3 getCorner(int index) const;
    glm::vec3 getNormal() const;

    std::array<glm::vec3, 3> getCorners() const;

    /**
     * @brief cartesianToBarycentric convert a point in a triangle in cartesian coordinates to barycentric coordinates
     * @param v the point in cartesian coordinates
     * @param a, b, c the resulting barycentric coordinates with respect to the triangle
     * @return true if successful, false if triangle is not valid
     */
    bool cartesianToBarycentric(const glm::vec2& v, double& a, double& b, double& c) const;

    double interpolateHeight(float x, float y) const;

    bool operator ==(const Triangle& other) const;
    Triangle& operator =(const Triangle &right);

private:
    std::array<glm::vec3, 3> mCorners;
};

#endif
