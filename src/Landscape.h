#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "Triangle.h"
#include "utils/Jpeg.h"

#include <GL/glut.h>
#include <vector>
#include <random>

/**
 @brief A landscape consisting of a mesh of triangles, regular in x and y, with
        changing heights in z.
 */
class Landscape
{
public:
    enum class Type
    {
        FLAT,
        RANDOM,
        FILE
    };

    Landscape();
    ~Landscape() = default;

    void generate(Type type);

    std::vector<Triangle> get();
    //void getLocalEnvironment(double x, double y, double& altitude, Vec3& surfaceNormal);
    void getLocalEnvironment(float x, float y, float& altitude, glm::vec3& surfaceNormal);
    double getHeight(double x, double y);
    double getHeight2(double x, double y) const;

    Triangle findTriangle(double x, double y) const;

    bool isCurrentTriangle(const Triangle& triangle) const;

    const GLfloat* getMaterialSpecular();
    const GLfloat* getMaterialShininess();

    void draw(glm::vec2 position, float radius);

protected:
    bool loadHeightmap(const std::string& filename);

    void generateSupportPoints();
    void interpolateTriangles();
    double interpolateBilinear(float x, float y, glm::vec3 x1y1, glm::vec3 x1y2, glm::vec3 x2y1, glm::vec3 x2y2) const;

    void generateRandomSurface();
    void generateFlatSurface();
    void generateHeightMapSurface();

    Type mType = Type::FLAT;
    const double pi = 3.1415926536;
    int mDimX = 101;
    int mDimY = 101;
    std::vector<std::vector<double>> mSupportPoints; // spacing between points is 10m
    std::vector<Triangle> mTriangles;
    double mMaxDeviationBetweenSupportPoints = 3.0;
    double mScale = 10.0;
    Triangle mCurrentTriangle;

    std::array<GLfloat, 4> mat_specular =
                    { 1.0, 1.0, 1.0, 1.0 };
    std::array<GLfloat, 1> mat_shininess =
                    { 50.0 };

    Jpeg mHeightMap;

};
#endif
