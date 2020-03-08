#include "Landscape.h"

#include "Utils.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#define DEBUG 0

Landscape::Landscape()
{
}

void Landscape::generate(Type type)
{
    mType = type;
    generateSupportPoints();
    interpolateTriangles();
}

std::vector<Triangle> Landscape::get()
{
    return mTriangles;
}

void Landscape::generateSupportPoints()
{
    mSupportPoints.clear();
    if (mType == Type::FLAT)
    {
        generateFlatSurface();
    }
    else if (mType == Type::RANDOM)
    {
        generateRandomSurface();
    }
    else if (mType == Type::FILE)
    {
        generateHeightMapSurface();
    }
}

void Landscape::generateRandomSurface()
{
    mScale = 10.0;
    std::mt19937 mRandomGenerator;
    for (int x = 0; x < mDimX; ++x)
    {
        std::vector<double> row;
        for (int y = 0; y < mDimY; ++y)
        {
            // consider available neighbours: (x,y-1),(x-1, y-1), (x-1, y), (x-1, y+1)
            if (x == 0 && y == 0)
            {
                row.push_back(0);
            }
            else if (x == 0)
            {
                // neighbours: only (x, y-1)
                // x x
                // o x
                // v x
                double center = row[y - 1];
                std::uniform_real_distribution<> dis(center - mMaxDeviationBetweenSupportPoints, center + mMaxDeviationBetweenSupportPoints);
                double newHeight = dis(mRandomGenerator);
                row.push_back(newHeight);
            }
            else if (y == 0)
            {
                // neighbours: only (x-1, y), (x-1, y+1)
                // v x x
                // v o x
                double center = (mSupportPoints[x - 1][y] + mSupportPoints[x - 1][y + 1]) / 2.0;
                std::uniform_real_distribution<> dis(center - mMaxDeviationBetweenSupportPoints, center + mMaxDeviationBetweenSupportPoints);
                double newHeight = dis(mRandomGenerator);
                row.push_back(newHeight);
            }
            else
            {
                // neighbours: only (x-1, y-1), (x-1, y), (x-1,y+1), (x, y-1)
                // v x x
                // v o x
                // v v x
                double center = (mSupportPoints[x - 1][y - 1] + mSupportPoints[x - 1][y] +
                                mSupportPoints[x - 1][y + 1] + row[y - 1]) / 4.0;
                std::uniform_real_distribution<> dis(center - mMaxDeviationBetweenSupportPoints, center + mMaxDeviationBetweenSupportPoints);
                double newHeight = dis(mRandomGenerator);
                row.push_back(newHeight);
            }
        }
        mSupportPoints.push_back(row);
    }
}

void Landscape::generateFlatSurface()
{
    mScale = 10.0;
    for (int x = 0; x < mDimX; ++x)
    {
        std::vector<double> row;
        for (int y = 0; y < mDimY; ++y)
        {
            row.push_back(0);
        }
        mSupportPoints.push_back(row);
    }
}

void Landscape::generateHeightMapSurface()
{
    std::string filename = "images/Terrain/terrain01_1081x1081.jpeg";
//    std::string filename = "images/Terrain/Garmisch.jpeg";
    mScale = 2.0;
    bool ret = loadHeightmap(filename);
    if (ret)
    {
        unsigned char* src = mHeightMap.data();
        for (int x = 0; x < mHeightMap.width(); ++x)
        {
            std::vector<double> row;
            for (int y = 0; y < mHeightMap.height(); ++y)
            {
                row.push_back(*src);
                src++;
            }
            mSupportPoints.push_back(row);
        }
    }
    else
    {
        std::cout << "ERROR: Failed to load heightmap '" << filename << "'" << std::endl;
        generateFlatSurface();
    }
}

void Landscape::interpolateTriangles()
{
    mTriangles.clear();
    for (int x = 0; x < mSupportPoints.size() - 1; ++x)
    {
        for (int y = 0; y < mSupportPoints[x].size() - 1; ++y)
        {
            {
                Triangle t;
                t.setCorner(0, glm::vec3(x * mScale, y * mScale, mSupportPoints[x][y]));
                t.setCorner(1, glm::vec3((x + 1) * mScale, (y + 1) * mScale, mSupportPoints[x + 1][y + 1]));
                t.setCorner(2, glm::vec3(x * mScale, (y + 1) * mScale, mSupportPoints[x][y + 1]));
                mTriangles.push_back(t);
            }
            {
                Triangle t;
                t.setCorner(0, glm::vec3(x * mScale, y * mScale, mSupportPoints[x][y]));
                t.setCorner(1, glm::vec3((x + 1) * mScale, y * mScale, mSupportPoints[x + 1][y]));
                t.setCorner(2, glm::vec3((x + 1) * mScale, (y + 1) * mScale, mSupportPoints[x + 1][y + 1]));
                mTriangles.push_back(t);
            }
        }
    }
    for (auto& triangle : mTriangles)
    {
        glm::vec3 normal = triangle.getNormal();
        mTriangleNormals.push_back(normal);
    }
}

double Landscape::interpolateBilinear(float x, float y, glm::vec3 x1y1, glm::vec3 x1y2, glm::vec3 x2y1, glm::vec3 x2y2) const
{
    float dx2x1 = (x2y1 - x1y1)[0];
    if (dx2x1 == 0) return 0;
    float dy2y1 = (x1y2 - x1y1)[1];
    if (dy2y1 == 0) return 0;
    float dx2x = x2y1[0] - x;
    float dy2y = x1y2[1] - y;
    float dx1x = x1y1[0] - x;
    float dy1y = x1y1[1] - y;
#if DEBUG
    std::cout << "dx2x: " << dx2x
              << "; dy2y: " << dy2y
              << "; dx1x: " << dx1x
              << "; dy1y: " << dy1y
              << "; dx2x1: " << dx2x1
              << "; dy2y1: " << dy2y1
              << "; x1y1: " << x1y1[2]
              << "; x2y1: " << x2y1[2]
              << "; x1y2: " << x1y2[2]
              << "; x2y2: " << x2y2[2]
              << std::endl;
#endif
    return (x1y1[2] * dx2x * dy2y -
            x2y1[2] * dx1x * dy2y -
            x1y2[2] * dx2x * dy1y +
            x2y2[2] * dx1x * dy1y) / dx2x1 / dy2y1;
}

double Landscape::getHeight2(double x, double y) const
{
    if (x < 0) return 0.0;
    if (y < 0) return 0.0;
    if (x > 100 * mScale) return 0;
    if (y > 100 * mScale) return 0;
    int scale = (int)mScale;
    int indexX = x / scale;
    int indexY = y / scale;
//    std::cout << "interpolate: " << x << "/" << y << " -> "
//            << indexX << ", " << (indexX + 1) * mScale << ", "
//            << indexY << ", " << (indexY + 1) * mScale
//            << std::endl;
    //std::cout << mSupportPoints[indexX][indexY] << std::endl;
    return interpolateBilinear(x, y, glm::vec3(indexX, indexY, mSupportPoints[indexX][indexY]),
                    glm::vec3(indexX * mScale, (indexY + 1) * mScale, mSupportPoints[indexX][indexY + 1]),
                    glm::vec3((indexX + 1) * mScale, indexY, mSupportPoints[indexX + 1][indexY]),
                    glm::vec3((indexX + 1) * mScale, (indexY + 1) * mScale, mSupportPoints[indexX + 1][indexY + 1]));
}

//void Landscape::getLocalEnvironment(double x, double y, double& altitude, Vec3& surfaceNormal)
//{
//    Triangle t = findTriangle(x, y);
//    mCurrentTriangle = Triangle(t);
//    altitude = t.interpolateHeight(x, y);
//    surfaceNormal = t.getNormal();
//}

void Landscape::getLocalEnvironment(float x, float y, float& altitude, glm::vec3& surfaceNormal)
{
    Triangle t = findTriangle(x, y);
    mCurrentTriangle = Triangle(t);
    altitude = t.interpolateHeight(x, y);
    surfaceNormal = t.getNormal();

#if DEBUG
    std::cout << "local environment for (" << x << "/" << y << "):"
                    << "\t" << "Triangle: " << t.getCorner(0) << " " << t.getCorner(1) << " " << t.getCorner(2)
                    << "\t" << "Normal: " << surfaceNormal
                    << std::endl;
#endif
}

double Landscape::getHeight(double x, double y)
{
    const Triangle& t = findTriangle(x, y);
    double result = t.interpolateHeight(x, y);
    return result;
}

Triangle Landscape::findTriangle(double x, double y) const
{
    glm::vec2 pos(x, y);
    // find triangle via polar coordinates
    // origin:
    glm::vec2 origin((int) (x / mScale) * mScale, (int) (y / mScale) * mScale);
    glm::vec2 diff = pos - origin;
    double rho = 0;
    if (diff[1] == 0.0)
    {
        if (diff[0] > 0) rho = 90.0;
        else rho = 270.0;
    }
    else
    {
        rho = atan(diff[0] / diff[1]) / pi * 180.0;
    }
    if (rho < 0) rho += 360.0;

    //std::cout << "pos: " << pos << "; origin: " << origin << "; rho: " << rho << "; diff: " << diff << std::endl;
    int scale = (int)mScale;
    int indexX = x / scale;
    int indexY = y / scale;

    Triangle t;
    if (rho <= 45.0) // upper left triangle
    {
        t.setCorner(0, glm::vec3(indexX * mScale, indexY * mScale, mSupportPoints[indexX][indexY]));
        t.setCorner(1, glm::vec3((indexX + 1) * mScale, (indexY + 1) * mScale, mSupportPoints[indexX + 1][indexY + 1]));
        t.setCorner(2, glm::vec3(indexX * mScale, (indexY + 1) * mScale, mSupportPoints[indexX][indexY + 1]));
    }
    else // lower right triangle
    {
        t.setCorner(0, glm::vec3(indexX * mScale, indexY * mScale, mSupportPoints[indexX][indexY]));
        t.setCorner(1, glm::vec3((indexX + 1) * mScale, indexY * mScale, mSupportPoints[indexX + 1][indexY]));
        t.setCorner(2, glm::vec3((indexX + 1) * mScale, (indexY + 1) * mScale, mSupportPoints[indexX + 1][indexY + 1]));
    }
    return t;
}

bool Landscape::isCurrentTriangle(const Triangle& triangle) const
{
    return (mCurrentTriangle == triangle);
}

const GLfloat* Landscape::getMaterialSpecular()
{
    return &mat_specular[0];
}

const GLfloat* Landscape::getMaterialShininess()
{
    return &mat_shininess[0];
}

void Landscape::draw(glm::vec2 position, float radius)
{
    unsigned long start = Utils::clockTimeMs();

    for (size_t i = 0; i < mTriangles.size(); ++i)
    {
        auto& triangle = mTriangles[i];
        glm::vec3 corner = triangle.getCorner(0);
//        460ms
//        glm::vec2 diff = position - glm::vec2(corner.x, corner.y);
//        if (radius * radius < diff.length())
//            continue;

//        220ms
//        if (pow(radius, 2) < (pow(position.x - corner.x, 2) + pow(position.y - corner.y, 2)))
//            continue;

        float dx = position.x - corner.x;
        float dy = position.y - corner.y;
        if (radius * radius < (dx * dx + dy * dy))
            continue;
        glBegin(GL_TRIANGLES);
        glm::vec3 p;
        for (int vertex = 0; vertex < 3; ++vertex)
        {
            if (vertex == 0)
            {
                glTexCoord2i(0, 0);
            }
            else if (vertex == 1)
            {
                glTexCoord2i(1, 1);
            }
            else
            {
                glTexCoord2i(0, 1);
            }

            p = triangle.getCorner(vertex);
            glVertex3f(p.x, p.y, p.z);
        }
        glm::vec3 normalVec = mTriangleNormals[i / 3];
        glNormal3fv(glm::value_ptr(normalVec));
        glEnd();
    }
    unsigned long end = Utils::clockTimeMs();
    if (end - start > 16)
    {
        std::cout << "WARNING: Landscape slow: " << end - start << "ms" << std::endl;
    }
}

void Landscape::drawNormals(glm::vec2 position, float radius)
{
    Triangle localTriangle = findTriangle(position.x,  position.y);
    for (const auto& triangle : mTriangles)
    {
        glm::vec3 corner = triangle.getCorner(0);
        float dx = position.x - corner.x;
        float dy = position.y - corner.y;

        if (radius > 0)
        {
            if (radius * radius < (dx * dx + dy * dy))
                continue;
        }
        else
        {
            if (triangle != localTriangle) continue;
        }

        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();

        glPushAttrib( GL_ENABLE_BIT );
        glDisable( GL_LIGHTING );

        glColor3f( 1.0f, 1.0f, 0.0f );// Yellow

        const float factor = 2.0;
        glBegin( GL_LINES );
#if 1
        glm::vec3 p = triangle.getIncircleCenter();
        glm::vec3 normalVec = triangle.getNormal();
        glVertex3f(p.x, p.y, p.z);
        glVertex3fv(glm::value_ptr(p + normalVec * factor));
//        std::cout << p.x << " " << p.y << " " << p.z << "\n";
#else
        for (int i = 0 ; i < 3; ++i)
        {
            glm::vec3 p = triangle.getCorner(i);
            glm::vec3 normalVec = triangle.getNormal();

            glVertex3f(p.x, p.y, p.z);
            glVertex3fv(glm::value_ptr(p + normalVec * factor));
//            std::cout << i << " " << p.x << " " << p.y << " " << p.z << " - " << normalVec.x << " " << normalVec.y << " " << normalVec.z << "\n";
        }
#endif
        glEnd();

        glPopAttrib();

        glPopMatrix();
    }
}

bool Landscape::loadHeightmap(const std::string& filename)
{
    int ret = mHeightMap.load(filename);
    if (ret != 0)
    {
        return false;
    }

    return true;
}
