#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "../Utils.h"

#include "gtest/gtest.h"

/*
 * Link for an online 3D vector rotation tool:
 * http://www.nh.cas.cz/people/lazar/celler/online_tools.php
 */
namespace
{
    class QuaternionTest: public ::testing::Test
    {
    protected:
        QuaternionTest()
        {
            // You can do set-up work for each test here.
        }

        virtual ~QuaternionTest()
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

    // Objects declared here can be used by all tests in the test case for Quaternion.
    TEST(QuaternionTest, BasicQuaternion)
    {
        const float epsilon = 1e-6;

        glm::vec3 forward(1.0f, 0.0f, 0.0f);
        glm::vec3 side(0.0f, 1.0f, 0.0f);
        glm::vec3 up(0.0f, 0.0f, 1.0f);

        float angle = 45.f; // degrees

        // quat around up:
        glm::quat rot = glm::angleAxis(glm::radians(angle), glm::vec3(0, 0, 1));

        // rotate the vector part of a quat by rotation as defined in the quat
        glm::quat vec(0.f, glm::vec3(1, 0, 0));
        glm::quat res = rot * vec * glm::inverse(rot);

        EXPECT_LE(fabs(res.x - sin(glm::radians(angle))), epsilon);
        EXPECT_LE(fabs(res.y - sin(glm::radians(angle))), epsilon);
        EXPECT_LE(fabs(res.z), epsilon);
    }

    TEST(QuaternionTest, PitchRollYaw)
    {
        glm::quat rot = Utils::quatFromEuler(20, 45, 90);

        float roll = glm::atan(2.f * (rot.x * rot.y + rot.w * rot.z), rot.w * rot.w + rot.x * rot.x - rot.y * rot.y - rot.z * rot.z);
        float pitch = glm::atan(2.f * (rot.y * rot.z + rot.w * rot.x), rot.w * rot.w - rot.x * rot.x - rot.y * rot.y + rot.z * rot.z);
        float yaw = glm::asin(glm::clamp(-2.f * (rot.x * rot.z - rot.w * rot.y), -1.f, 1.f));

        std::cout << "roll: " << glm::degrees(roll) << std::endl;
        std::cout << "pitch: " << glm::degrees(pitch) << std::endl;
        std::cout << "yaw: " << glm::degrees(yaw) << std::endl;
    }

    TEST(QuaternionTest, Rotation)
    {
        const float epsilon = 1e-6;

        glm::quat forward(0, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat side(0, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat up(0, glm::vec3(0.0f, 0.0f, 1.0f));

        // all rotations are counter clockwise in this right hand system
        float pitch = 45.0f;
        float roll = 90.0f;
        float yaw = 45.0f;

//        std::cout << "Yaw rotation" << std::endl;
        glm::quat rotYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(up.x, up.y, up.z));
//        std::cout << "rotYaw: " << rotYaw << std::endl;
        forward = rotYaw * forward * glm::inverse(rotYaw);
        side = rotYaw * side * glm::inverse(rotYaw);

        // forward: (sin(yaw), sin(yaw), 0)
//        std::cout << "forward: " << forward << std::endl;
        EXPECT_LE(fabs(forward.x - sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(forward.y - sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(forward.z), epsilon);

        // side: (-sin(yaw), sin(yaw), 0)
//        std::cout << "side: " << side << std::endl;
        EXPECT_LE(fabs(side.x + sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(side.y - sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(side.z), epsilon);

//        std::cout << "Roll rotation" << std::endl;
        glm::quat rotRoll = glm::angleAxis(glm::radians(roll), glm::vec3(forward.x, forward.y, forward.z));
        side = rotRoll * side * glm::inverse(rotRoll);
        up = rotRoll * up * glm::inverse(rotRoll);

        // side: (0, 0, 1)
//        std::cout << "side: " << side << std::endl;
        EXPECT_LE(fabs(side.x), epsilon);
        EXPECT_LE(fabs(side.y), epsilon);
        EXPECT_LE(fabs(side.z - 1.f), epsilon);

        // up: (sin(45), -sin(45), 0)
//        std::cout << "up: " << up << std::endl;
        EXPECT_LE(fabs(up.x - sin(glm::radians(45.f))), epsilon);
        EXPECT_LE(fabs(up.y + sin(glm::radians(45.f))), epsilon);
        EXPECT_LE(fabs(up.z), epsilon);

//        std::cout << "Pitch rotation" << std::endl;
        glm::quat rotPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(side.x, side.y, side.z));
        forward = rotPitch * forward * glm::inverse(rotPitch);
        up = rotPitch * up * glm::inverse(rotPitch);

        // forward: (sin(yaw), -sin(yaw), 0) -> (0, 1, 0)
//        std::cout << "forward: " << forward << std::endl;
        EXPECT_LE(fabs(forward.x), epsilon);
        EXPECT_LE(fabs(forward.y - 1.f), epsilon);
        EXPECT_LE(fabs(forward.z), epsilon);

        // up: (sin(45), sin(45), 0) -> (1, 0, 0)
//        std::cout << "up: " << up << std::endl;
        EXPECT_LE(fabs(up.x - 1.f), epsilon);
        EXPECT_LE(fabs(up.y), epsilon);
        EXPECT_LE(fabs(up.z), epsilon);

//        std::cout << "Multiple rotation" << std::endl;

        glm::quat test(0, glm::vec3(1, 0, 0));
//        std::cout << "test: " << test << std::endl;
        glm::quat res = rotYaw * test * glm::inverse(rotYaw);
//        std::cout << "res_yaw: " << res << std::endl;

        EXPECT_LE(fabs(res.x - sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(res.y - sin(glm::radians(yaw))), epsilon);
        EXPECT_LE(fabs(res.z), epsilon);

        res = rotRoll * res * glm::inverse(rotRoll);
        glm::quat res2 = rotRoll * rotYaw * test * glm::inverse(rotYaw) * glm::inverse(rotRoll);
//        std::cout << "res: " << res << std::endl;
//        std::cout << "res2: " << res2 << std::endl;

        EXPECT_LE(fabs(res.x - res2.x), epsilon);
        EXPECT_LE(fabs(res.y - res2.y), epsilon);
        EXPECT_LE(fabs(res.z - res2.z), epsilon);

        test = rotPitch * rotRoll * rotYaw * test * glm::inverse(rotYaw) * glm::inverse(rotRoll) * glm::inverse(rotPitch);
//        std::cout << "test: " << test << std::endl;
        EXPECT_LE(fabs(test.x), epsilon);
        EXPECT_LE(fabs(test.y - 1.f), epsilon);
        EXPECT_LE(fabs(test.z), epsilon);

//        glm::quat ThreeDRot = rotPitch * rotRoll * rotYaw;
//        std::cout << "ThreeDRot: " << ThreeDRot << std::endl;
    }

    TEST(QuaternionTest, QuatFromEuler)
    {
        const float epsilon = 1e-6;

        float roll = 90.f;
        float pitch = 45.f;
        float yaw = 45.f;

        glm::quat rot = Utils::quatFromEuler(roll, pitch, yaw);

        EXPECT_LE(fabs(glm::length(rot) - 1.f), epsilon);
        EXPECT_LE(fabs(rot.w - 0.5), epsilon);
        EXPECT_LE(fabs(rot.x - 0.5), epsilon);
        EXPECT_LE(fabs(rot.y - 0.5), epsilon);
        EXPECT_LE(fabs(rot.z - 0.5), epsilon);
    }

    TEST(QuaternionTest, Rotation2)
    {
        const float epsilon = 1e-6;

        glm::quat forward(0, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat side(0, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat up(0, glm::vec3(0.0f, 0.0f, 1.0f));

        // all rotations are counter clockwise in this right hand system
        float pitch = 48.0f;
        float roll = 36.0f;
        float yaw = 17.0f;

        const bool showOutput = false;

        if (showOutput)
            std::cout << "Yaw rotation" << std::endl;
        glm::quat rotYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(up.x, up.y, up.z));
        if (showOutput)
            std::cout << "rotYaw: " << rotYaw << std::endl;
        forward = rotYaw * forward * glm::inverse(rotYaw);
        side = rotYaw * side * glm::inverse(rotYaw);

        // forward: (0.956305, 0.292372, 0)
        if (showOutput)
            std::cout << "forward: " << forward << std::endl;
        EXPECT_LE(fabs(forward.x - 0.956305), epsilon);
        EXPECT_LE(fabs(forward.y - 0.292372), epsilon);
        EXPECT_LE(fabs(forward.z), epsilon);

        // side: (-0.292372, 0.956305, 0)
        if (showOutput)
            std::cout << "side: " << side << std::endl;
        EXPECT_LE(fabs(side.x + 0.292372), epsilon);
        EXPECT_LE(fabs(side.y - 0.956305), epsilon);
        EXPECT_LE(fabs(side.z), epsilon);

        if (showOutput)
            std::cout << "Roll rotation" << std::endl;
        glm::quat rotRoll = glm::angleAxis(glm::radians(roll), glm::vec3(forward.x, forward.y, forward.z));
        side = rotRoll * side * glm::inverse(rotRoll);
        up = rotRoll * up * glm::inverse(rotRoll);

        // side: (-0.236534, 0.773667, 0.587785)
        if (showOutput)
            std::cout << "side: " << side << std::endl;
        EXPECT_LE(fabs(side.x + 0.236534), epsilon);
        EXPECT_LE(fabs(side.y - 0.773667), epsilon);
        EXPECT_LE(fabs(side.z - 0.587785), epsilon);

        // up: (0.171852, -0.562102, 0.809017)
        if (showOutput)
            std::cout << "up: " << up << std::endl;
        EXPECT_LE(fabs(up.x - 0.171852), epsilon);
        EXPECT_LE(fabs(up.y + 0.562102), epsilon);
        EXPECT_LE(fabs(up.z - 0.809017), epsilon);

        if (showOutput)
            std::cout << "Pitch rotation" << std::endl;
        glm::quat rotPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(side.x, side.y, side.z));
        forward = rotPitch * forward * glm::inverse(rotPitch);
        up = rotPitch * up * glm::inverse(rotPitch);

        // forward: (0.512182, 0.613358, -0.601217)
        if (showOutput)
            std::cout << "forward: " << forward << std::endl;
        EXPECT_LE(fabs(forward.x - 0.512182), epsilon);
        EXPECT_LE(fabs(forward.y - 0.613358), epsilon);
        EXPECT_LE(fabs(forward.z + 0.601217), epsilon);

        // up: (0.825664, -0.158845, 0.541338)
        if (showOutput)
            std::cout << "up: " << up << std::endl;
        EXPECT_LE(fabs(up.x - 0.825664), epsilon);
        EXPECT_LE(fabs(up.y + 0.158845), epsilon);
        EXPECT_LE(fabs(up.z - 0.541338), epsilon);

        if (showOutput)
            std::cout << "Multiple rotation" << std::endl;

        glm::quat test(0, glm::vec3(1, 0, 0));
        if (showOutput)
            std::cout << "test: " << test << std::endl;

        test = rotPitch * rotRoll * rotYaw * test * glm::inverse(rotYaw) * glm::inverse(rotRoll) * glm::inverse(rotPitch);

        // test: (0.512182, 0.613358, -0.601217)
        if (showOutput)
            std::cout << "test: " << test << std::endl;
        EXPECT_LE(fabs(test.x - 0.512182), epsilon);
        EXPECT_LE(fabs(test.y - 0.613358), epsilon);
        EXPECT_LE(fabs(test.z + 0.601217), epsilon);

        glm::quat test2(0, glm::vec3(0.7, 1.3, -1.2));
        test2 = rotPitch * rotRoll * rotYaw * test2 * glm::inverse(rotYaw) * glm::inverse(rotRoll) * glm::inverse(rotPitch);
        if (showOutput)
            std::cout << "test2: " << test2 << std::endl;
        // test2: (-0.939764, 1.625731, -0.306337)
        EXPECT_LE(fabs(test2.x + 0.939764), epsilon);
        EXPECT_LE(fabs(test2.y - 1.625731), epsilon);
        EXPECT_LE(fabs(test2.z + 0.306337), epsilon);
    }

    TEST(QuaternionTest, QuatFromTwoVectors)
    {
        const float epsilon = 1e-6;
        glm::vec3 u(0.2, 1.2, -0.3);
        u = glm::normalize(u);
        glm::vec3 v(0.8, -0.7, 3.2);
        v = glm::normalize(v);
        glm::quat rot = Utils::quatFromTwoVectors(u, v);
        glm::quat result = rot * glm::quat(0, u) * glm::inverse(rot);
        glm::vec3 res = glm::axis(result);
//        std::cout << "res: " << res << std::endl;
        EXPECT_LE(fabs(v.x - res.x), epsilon);
        EXPECT_LE(fabs(v.y - res.y), epsilon);
        EXPECT_LE(fabs(v.z - res.z), epsilon);
    }

    TEST(QuaternionTest, MatTest)
    {
        const float epsilon = 1e-6;
        // mat(col1, col2, col3)
        glm::mat3 mat(1, 1, 0, 0, 2, 2, 3, 0, 3);
        // =>
        // (1, 0, 3)
        // (1, 2, 0)
        // (0, 2, 3)
        glm::vec3 vec(1, 0, 0);
        vec = mat * vec;
        EXPECT_LE(fabs(vec.x - 1), epsilon);
        EXPECT_LE(fabs(vec.y - 1), epsilon);
        EXPECT_LE(fabs(vec.z - 0), epsilon);

    }
}
