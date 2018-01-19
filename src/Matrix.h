#ifndef MATRIX_H
#define MATRIX_H

#include "glm/vec3.hpp"
#include <iostream>
#include <array>

class Matrix
{
public:
    Matrix();
    Matrix(const glm::vec3& row1, const glm::vec3& row2, const glm::vec3& row3);
    ~Matrix() = default;

    void setToUnity();

    glm::vec3 operator*(const glm::vec3& rhs) const;

    void setRotationMatrix(glm::vec3 rotationAxis, double angle);

    friend std::ostream & operator <<(std::ostream & out, const Matrix &right)
    {
        out << "Matrix(";
        for (const auto& row : right.indices)
        {
            out << "{";
            for (const auto& col : row)
            {
                out << col << " ";
            }
            out << "}";
        }
        out << ")";
        return out;
    }

    // [row][column]
    std::array<std::array<double, 4>, 4> indices;
};

#endif
