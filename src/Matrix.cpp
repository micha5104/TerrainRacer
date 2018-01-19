#include "Matrix.h"
#include "glm/gtx/quaternion.hpp"

Matrix::Matrix()
{
    setToUnity();
}

Matrix::Matrix(const glm::vec3& row1, const glm::vec3& row2, const glm::vec3& row3)
{
    indices[0][0] = row1.x;
    indices[0][1] = row1.y;
    indices[0][2] = row1.z;
    indices[1][0] = row2.x;
    indices[1][1] = row2.y;
    indices[1][2] = row2.z;
    indices[2][0] = row3.x;
    indices[2][1] = row3.y;
    indices[2][2] = row3.z;
}

void Matrix::setToUnity()
{
    for (auto& row : indices)
    {
        for (auto& col : row)
        {
            col = 0.0;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        indices[i][i] = 1.0;
    }
}

glm::vec3 Matrix::operator*(const glm::vec3& rhs) const
{
    glm::vec3 result = glm::vec3(indices[0][0] * rhs.x + indices[0][1] * rhs.y + indices[0][2] * rhs.z,
                    indices[1][0] * rhs.x + indices[1][1] * rhs.y + indices[1][2] * rhs.z,
                    indices[2][0] * rhs.x + indices[2][1] * rhs.y + indices[2][2] * rhs.z);
    return result;
}

void Matrix::setRotationMatrix(glm::vec3 rotationAxis, double angle)
{
    const glm::vec3 u = glm::normalize(rotationAxis);
    const double c = cos(angle);
    const double s = sin(angle);
    indices[0][0] = c + u.x * u.x * (1.0 - c);
    indices[0][1] = u.x * u.y * (1.0 - c) - u.z * s;
    indices[0][2] = u.x * u.z * (1.0 - c) + u.y * s;
    indices[1][0] = u.y * u.x * (1.0 - c) + u.z * s;
    indices[1][1] = c + u.y * u.y * (1.0 - c);
    indices[1][2] = u.y * u.z * (1.0 - c) - u.x * s;
    indices[2][0] = u.z * u.x * (1.0 - c) - u.y * s;
    indices[2][1] = u.z * u.y * (1.0 - c) + u.x * s;
    indices[2][2] = c + u.z * u.z * (1.0 - c);
}
