#include "Triangle.h"

#include <iostream>

constexpr auto EPSILON = (1e-8);

Triangle::Triangle()
{
    const glm::vec3 t;
    mCorners[0] = t;
    mCorners[1] = t;
    mCorners[2] = t;
}

Triangle::Triangle(const Triangle& other)
{
    for (int i = 0; i < 3; ++i)
    {
        mCorners[i] = other.getCorner(i);
    }
}

std::array<glm::vec3, 3> Triangle::getCorners() const
{
    return mCorners;
}

void Triangle::setCorner(int index, const glm::vec3& coord)
{
    mCorners[index] = coord;
}

glm::vec3 Triangle::getCorner(int index) const
{
    return mCorners[index];
}

glm::vec3 Triangle::getNormal() const
{
    glm::vec3 a = mCorners[0];
    glm::vec3 b = mCorners[1];
    glm::vec3 c = mCorners[2];
    glm::vec3 res = glm::cross(b - a, c - a);
    return glm::normalize(res);
}

glm::vec3 Triangle::getIncircleCenter() const
{
    // see https://en.wikipedia.org/wiki/Incenter#Barycentric_coordinates
    double a = glm::length(mCorners[2] - mCorners[1]);
    double b = glm::length(mCorners[0] - mCorners[2]);
    double c = glm::length(mCorners[1] - mCorners[0]);
    double P = a + b + c;
    glm::vec3 barycentric(a / P, b / P, c / P);
    return barycentricToCartesian(barycentric);
}

bool Triangle::cartesianToBarycentric(const glm::vec2& v, double& a, double& b, double& c) const
{
    // see https://en.wikipedia.org/wiki/Barycentric_coordinate_system
    double det = (mCorners[1][1] - mCorners[2][1]) * (mCorners[0][0] - mCorners[2][0]) +
                    (mCorners[2][0] - mCorners[1][0]) * (mCorners[0][1] - mCorners[2][1]);

    if (det == 0.0)
    {
        std::cerr << "Triangle: cannot convert to barycentric coordinates - triangle is vot valid" << std::endl;
        return false;
    }

    a = ((mCorners[1][1] - mCorners[2][1]) * (v[0] - mCorners[2][0]) +
                    (mCorners[2][0] - mCorners[1][0]) * (v[1] - mCorners[2][1])) / det;

    b = ((mCorners[2][1] - mCorners[0][1]) * (v[0] - mCorners[2][0]) +
                    (mCorners[0][0] - mCorners[2][0]) * (v[1] - mCorners[2][1])) / det;

    c = 1 - a - b;
    return true;
}

glm::vec3 Triangle::barycentricToCartesian(const glm::vec3& barycentric) const
{
    return barycentric.x * mCorners[0] + barycentric.y * mCorners[1] + barycentric.z * mCorners[2];
}

double Triangle::interpolateHeight(float x, float y) const
{
    // interpolate in the triangle
    // see https://en.wikipedia.org/wiki/Barycentric_coordinate_system
    double a, b, c;
    bool res = cartesianToBarycentric(glm::vec2(x, y), a, b, c);
    if (res == false)
    {
        std::cout << "Invalid triangle!" << std::endl;
    }
    return a * getCorner(0)[2] + b * getCorner(1)[2] + c * getCorner(2)[2];
}

bool Triangle::operator ==(const Triangle& other) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (fabs(mCorners[i].x - other.mCorners[i].x) > EPSILON)
        {
            return false;
        }
        else if (fabs(mCorners[i].y - other.mCorners[i].y) > EPSILON)
        {
            return false;
        }
        else if (fabs(mCorners[i].z - other.mCorners[i].z) > EPSILON)
        {
            return false;
        }
    }
    return true;
}

bool Triangle::operator !=(const Triangle& other) const
{
    return ((*this == other) == false);
}

Triangle& Triangle::operator =(const Triangle &right)
{
    mCorners = right.getCorners();
    return *this;
}
