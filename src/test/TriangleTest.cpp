#include "../Triangle.h"
#include "gtest/gtest.h"

namespace
{
    class TriangleTest : public ::testing::Test
    {
    protected:
        TriangleTest()
        {
            // You can do set-up work for each test here.
        }

        virtual ~TriangleTest()
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
    TEST(TriangleTest, InterpolateHeight)
    {
        const double epsilon = 1e-6;

        glm::vec3 a(0, 0, 0);
        glm::vec3 b(1, 0, 0);
        glm::vec3 c(1, 1, 1);
        Triangle triangle;
        triangle.setCorner(0, a);
        triangle.setCorner(1, b);
        triangle.setCorner(2, c);

        double x = 0.3;
        double y = 0.4;
        double height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - y), epsilon);

        x = 0;
        y = 0.8;
        height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - y), epsilon);

        x = 1;
        y = 0.2;
        height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - y), epsilon);

        a = glm::vec3(0, 0, 0);
        b = glm::vec3(0, 1, 0);
        c = glm::vec3(1, 1, 1);
        triangle.setCorner(0, a);
        triangle.setCorner(1, b);
        triangle.setCorner(2, c);

        x = 0.3;
        y = 0.4;
        height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - x), epsilon);

        x = 0;
        y = 0.8;
        height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - x), epsilon);

        x = 1;
        y = 0.2;
        height = triangle.interpolateHeight(x, y);
        EXPECT_LE(fabs(height - x), epsilon);
    }

    TEST(TriangleTest, Normal)
    {
        glm::vec3 a(1, 0, 0);
        glm::vec3 b(1, 1, 0);
        glm::vec3 c(0, 1, 0);
        Triangle triangle;
        triangle.setCorner(0, a);
        triangle.setCorner(1, b);
        triangle.setCorner(2, c);

        glm::vec3 normal = triangle.getNormal();
        EXPECT_EQ(normal.x, 0);
        EXPECT_EQ(normal.y, 0);
        EXPECT_EQ(normal.z, 1);
    }

    TEST(TriangleTest, Incircle)
    {
        const double epsilon = 1e-6;

        glm::vec3 p1(1, 0, 0);
        glm::vec3 p2(1, 1, 0);
        glm::vec3 p3(0, 1, 0);
        Triangle triangle;
        triangle.setCorner(0, p1);
        triangle.setCorner(1, p2);
        triangle.setCorner(2, p3);

        double a = glm::length(p3 - p2);
        double b = glm::length(p1 - p3);
        double c = glm::length(p2 - p1);
        double P = a + b + c;
        glm::vec2 incenter = glm::vec2( (a * p1.x + b * p2.x + c * p3.x) / P , (a * p1.y + b * p2.y + c * p3.y) / P );

        glm::vec3 value = triangle.getIncircleCenter();
        EXPECT_LE(fabs(value.x - incenter.x), epsilon);
        EXPECT_LE(fabs(value.y - incenter.y), epsilon);
        EXPECT_EQ(value.z, 0);
    }
}
