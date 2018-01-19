#ifndef LANDSCAPE_MOCK
#define LANDSCAPE_MOCK

#include "../Landscape.h"

class LandscapeMock : public Landscape
{
public:
    LandscapeMock() = default;

    std::vector<std::vector<double>> supportPoints()
    {
        return mSupportPoints;
    }

    std::vector<Triangle> triangles()
    {
        return mTriangles;
    }

    int dimX()
    {
        return mDimX;
    }

    int dimY()
    {
        return mDimY;
    }
};

#endif
