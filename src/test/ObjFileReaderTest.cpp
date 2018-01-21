#include "../ObjFileReader.h"
#include "gtest/gtest.h"

namespace
{
    class ObjFileReaderTest: public ::testing::Test
    {
    protected:
        ObjFileReaderTest()
        {
            // You can do set-up work for each test here.
        }

        virtual ~ObjFileReaderTest()
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

    // Objects declared here can be used by all tests in the test case for ObjFileReaderTest.
    TEST(ObjFileReaderTest, ReadFile)
    {
        ObjFileReader rdr;
        bool ret = rdr.loadFile("../Vehicle.obj");
        EXPECT_EQ(ret, true);
        auto objs = rdr.getObjects();
        EXPECT_EQ(objs.size(), 5);
        VertexObject chassis = objs["Chassis"];
        EXPECT_GT(chassis.getFaces().size(), 0);
    }

}
