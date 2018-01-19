#include "LandscapeMock.h"
#include "gtest/gtest.h"

namespace
{
    class LandscapeTest : public ::testing::Test
    {
    protected:
        LandscapeTest()
        {
            // You can do set-up work for each test here.
        }

        virtual ~LandscapeTest()
        {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        virtual void SetUp()
        {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        virtual void TearDown()
        {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }
    };

    // Objects declared here can be used by all tests in the test case for Landscape.
    TEST(LandscapeTest, FindTriangle)
    {
        LandscapeMock landscape;
        landscape.generate(Landscape::Type::FLAT);

        std::vector<std::vector<double>> supportPoints = landscape.supportPoints();
        EXPECT_EQ(supportPoints.size(), landscape.dimX());
        for (const auto& row : supportPoints)
        {
            EXPECT_EQ(row.size(), landscape.dimY());
        }
        EXPECT_EQ(landscape.triangles().size(), (landscape.dimX() - 1) * (landscape.dimY() - 1) * 2);
        EXPECT_EQ(landscape.get().size(), (landscape.dimX() - 1) * (landscape.dimY() - 1) * 2);

        // +---+---+
        // |  /|  /|
        // | / |X/ |
        // |/  |/  |
        // +---+---+
        // |  /|  /|
        // | / | / |
        // |/  |/  |
        // +---+---+
        Triangle t = landscape.findTriangle(11, 12);
        EXPECT_EQ(t.getCorner(0).x, 10);
        EXPECT_EQ(t.getCorner(0).y, 10);
        EXPECT_EQ(t.getCorner(1).x, 20);
        EXPECT_EQ(t.getCorner(1).y, 20);
        EXPECT_EQ(t.getCorner(2).x, 10);
        EXPECT_EQ(t.getCorner(2).y, 20);

        // +---+---+
        // |  /|  /|
        // | / | / |
        // |/  |/ X|
        // +---+---+
        // |  /|  /|
        // | / | / |
        // |/  |/  |
        // +---+---+
        t = landscape.findTriangle(12, 11);
        EXPECT_EQ(t.getCorner(0).x, 10);
        EXPECT_EQ(t.getCorner(0).y, 10);
        EXPECT_EQ(t.getCorner(1).x, 20);
        EXPECT_EQ(t.getCorner(1).y, 10);
        EXPECT_EQ(t.getCorner(2).x, 20);
        EXPECT_EQ(t.getCorner(2).y, 20);

        // +---+---+
        // |  /|  /|
        // | / | / |
        // |/  |/ X|
        // +---+---+
        t = landscape.findTriangle(15, 3);
        EXPECT_EQ(t.getCorner(0).x, 10);
        EXPECT_EQ(t.getCorner(0).y, 0);
        EXPECT_EQ(t.getCorner(1).x, 20);
        EXPECT_EQ(t.getCorner(1).y, 0);
        EXPECT_EQ(t.getCorner(2).x, 20);
        EXPECT_EQ(t.getCorner(2).y, 10);

        // +---+---+
        // |  /|X /|
        // | / | / |
        // |/  |/  |
        // +---+---+
        t = landscape.findTriangle(15, 8);
        EXPECT_EQ(t.getCorner(0).x, 10);
        EXPECT_EQ(t.getCorner(0).y, 0);
        EXPECT_EQ(t.getCorner(1).x, 20);
        EXPECT_EQ(t.getCorner(1).y, 10);
        EXPECT_EQ(t.getCorner(2).x, 10);
        EXPECT_EQ(t.getCorner(2).y, 10);
    }

}
