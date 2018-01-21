#include "Tank.h"
#include "Utils.h"
#include "ObjFileReader.h"
#include <glu.h>
#include <GL/glut.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp" // for glm::toMat4
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <array>

#define DEBUG 0

Tank::Tank()
{
    mPosition = glm::vec3(0.0, 0.0, 0.0);

    ObjFileReader rdr;
    bool ret = rdr.loadFile("./Vehicle.obj", 0.01);
    if (ret)
    {
        mObjects = rdr.getObjects();
        mModelLoaded = true;
    }
}

glm::vec3 Tank::move()
{
    glm::quat rot = Utils::quatFromEuler(mRoll, mPitch, mYaw);
    glm::quat direction = rot * glm::quat(0, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::inverse(rot);
    mPosition = mPosition + glm::axis(direction) * mVelocity;
    mPosition.z -= 0.1; // apply gravity
    return mPosition;
}

void Tank::draw()
{
    glPushMatrix();

    glm::quat eulerQuat = Utils::quatFromEuler(mRoll, mPitch, mYaw);
    glm::mat4 rotationMatrix = glm::toMat4(eulerQuat);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(mPosition.x, mPosition.y, mPosition.z));
    glm::mat4 mat = translationMatrix * rotationMatrix;
    glMultMatrixf(glm::value_ptr(mat));

    if (mModelLoaded)
    {
        drawModel(Model::File);
    }
    else
    {
        drawModel(Model::Cube);
    }

#if DEBUG
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();

    std::cout << "Tank:" << std::endl;
    std::cout << "\tPos: " << mPosition.x << " " << mPosition.y << " " << mPosition.z << std::endl;
    std::cout << "\tOri: " << mOrientation.x << " " << mOrientation.y << " " << mOrientation.z << std::endl;
    std::cout << "\tqua: " << eulerToQuat.x << " " << eulerToQuat.y << " " << eulerToQuat.z << std::endl;
    std::cout << "\tMat:" << std::endl;
    for (int row = 0; row < 4; ++row)
    {
        std::cout << "\t";
        for (int col = 0; col < 4; ++col)
        {
            std::cout << mat[col][row] << " ";
        }
        std::cout << std::endl;
    }
#endif
    glPopMatrix();
}

void Tank::accelerate(double acceleration)
{
    mVelocity += acceleration;
}

double Tank::velocity() const
{
    return mVelocity;
}

void Tank::stop()
{
    mVelocity = 0.0f;
}

void Tank::setPosition(glm::vec3 position)
{
    mPosition = position;
}

glm::vec3 Tank::position() const
{
    return mPosition;
}

void Tank::setOrientation(float pitch, float yaw, float roll)
{
    mPitch = pitch;
    mYaw = yaw;
    mRoll = roll;
}

void Tank::setRoll(float roll)
{
    while (roll >= 360.f)
        roll -= 360.f;
    while (roll < 0.f)
        roll += 360.f;
    mRoll = roll;
}

void Tank::setPitch(float pitch)
{
    while (pitch >= 360.f)
        pitch -= 360.f;
    while (pitch < 0.f)
        pitch += 360.f;
    mPitch = pitch;
}

void Tank::setYaw(float yaw)
{
    while (yaw >= 360.f)
        yaw -= 360.f;
    while (yaw < 0.f)
        yaw += 360.f;
    mYaw = yaw;
}

float Tank::pitch() const
{
    return mPitch;
}

float Tank::yaw() const
{
    return mYaw;
}

float Tank::roll() const
{
    return mRoll;
}

void Tank::drawModel(Model model)
{
    if (model == Model::Simple)
    {
        glBegin(GL_QUADS);

        // left side
        glVertex3f(-1.0, -0.5, 0.0);
        glVertex3f(-1.0, -0.5, 0.5);
        glVertex3f(1.0, -0.5, 0.5);
        glVertex3f(1.0, -0.5, 0.0);
        glNormal3f(0.0, -1.0, 0.0);

        // right side
        glVertex3f(-1.0, 0.5, 0.0);
        glVertex3f(-1.0, 0.5, 0.5);
        glVertex3f(1.0, 0.5, 0.5);
        glVertex3f(1.0, 0.5, 0.0);
        glNormal3f(0.0, 1.0, 0.0);

        // front
        glVertex3f(1.0, -0.5, 0.0);
        glVertex3f(1.0, -0.5, 0.5);
        glVertex3f(1.0, 0.5, 0.5);
        glVertex3f(1.0, 0.5, 0.0);
        glNormal3f(1.0, 0.0, 0.0);

        // back
        glVertex3f(-1.0, -0.5, 0.0);
        glVertex3f(-1.0, -0.5, 0.8);
        glVertex3f(-1.0, 0.5, 0.8);
        glVertex3f(-1.0, 0.5, 0.0);
        glNormal3f(-1.0, 0.0, 0.0);

        // hood
        glVertex3f(1.0, -0.5, 0.5);
        glVertex3f(1.0, 0.5, 0.5);
        glVertex3f(-0.2, 0.5, 0.5);
        glVertex3f(-0.2, -0.5, 0.5);
        glNormal3f(0.0, 0.0, 1.0);

        // screen
        glVertex3f(0.0, -0.5, 0.5);
        glVertex3f(0.0, 0.5, 0.5);
        glVertex3f(-0.4, 0.5, 0.8);
        glVertex3f(-0.4, -0.5, 0.8);
        glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.4, 0.0, 0.3)));
        glNormal3f(normal.x, normal.y, normal.z);

        // roof
        glVertex3f(-0.4, -0.5, 0.8);
        glVertex3f(-0.4, 0.5, 0.8);
        glVertex3f(-1.0, 0.5, 0.8);
        glVertex3f(-1.0, -0.5, 0.8);
        glNormal3f(0.0, 0.0, 1.0);
        glEnd();
    }
    else if (model == Model::File)
    {
//        std::array<VertexObject, 4> wheels =
//                        { mObjects["Wheel_FL"], mObjects["Wheel_FR"], mObjects["Wheel_BL"], mObjects["Wheel_BR"] };
        std::array<VertexObject, 1> wheels =
                        { mObjects["Wheel_FL"] };
        for (auto& wheel : wheels)
        {
            auto faces = wheel.getFaces();
            for (const auto& face : faces)
            {
                if (face.type == VertexObject::Type::Triangle)
                {
                    glBegin(GL_TRIANGLES);
                }
                else if (face.type == VertexObject::Type::Quad)
                {
                    glBegin(GL_QUADS);
                }
                else
                {
                    glBegin(GL_POLYGON);
                }

                const std::vector<glm::vec3>& vertices = face.vertices;
                const std::vector<glm::vec3>& normals = face.normals;
                for (int i = 0; i < vertices.size(); ++i)
                {
                    glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
                    glNormal3f(normals[i].x, normals[i].y, normals[i].z);
                }
                glEnd();
            }
        }

#if 0
        VertexObject chassis = mObjects["Chassis"];
        auto faces = chassis.getFaces();
        for (const auto& face : faces)
        {
            if (face.type == VertexObject::Type::Triangle)
            {
                glBegin(GL_TRIANGLES);
            }
            else if (face.type == VertexObject::Type::Quad)
            {
                glBegin(GL_QUADS);
            }
            else
            {
                glBegin(GL_POLYGON);
            }

            const std::vector<glm::vec3>& vertices = face.vertices;
            const std::vector<glm::vec3>& normals = face.normals;
            for (int i = 0; i < vertices.size(); ++i)
            {
                glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
                glNormal3f(normals[i].x, normals[i].y, normals[i].z);
            }
            glEnd();
        }
#endif
    }
    else
    {
        glPushMatrix();
        glTranslatef(0, 0, 0.5f);
        glutSolidCube(1.f);
        glPopMatrix();
    }
}

void Tank::rotateToMatchSurfaceNormal(const glm::vec3& surfaceNormal)
{
    // yaw never changes
#if DEBUG
    std::cout << "Tank orientation (euler): " << mRoll << " " << mPitch << " " << mYaw << std::endl;
#endif

    glm::quat toTank = Utils::quatFromEuler(mRoll, mPitch, mYaw);
    // get tank forward direction
    glm::quat forward(0, glm::vec3(1, 0, 0));
    forward = glm::normalize(toTank * forward * glm::inverse(toTank));

    // get tank side direction
    glm::quat side(0, glm::vec3(0, 1, 0));
    side = glm::normalize(toTank * side * glm::inverse(toTank));

    // get tank up direction
    glm::quat up(0, glm::vec3(0, 0, 1));
    up = glm::normalize(toTank * up * glm::inverse(toTank));

#if DEBUG
    std::cout << "Tank up:  " << up.x << " " << up.y << " " << up.z << std::endl;
    std::cout << "srf norm: " << surfaceNormal.x << " " << surfaceNormal.y << " " << surfaceNormal.z << std::endl;
#endif

    // get surface normal in (forward, side, up) coordinates

    /* Matrix * vec
     * ( 2 3 4)   (1)   (2)
     * ( 5 6 7) * (0) = (5)
     * ( 8 9 1)   (0)   (8)
     */
    glm::mat3 toFSU(forward.x, forward.y, forward.z,
                    side.x, side.y, side.z,
                    up.x, up.y, up.z);
    toFSU = glm::inverse(toFSU);
    glm::vec3 surfaceFSU = glm::normalize(toFSU * surfaceNormal);

    // get angle to modify roll to match surface plane

    // The (directed) angle from vector1 to vector2 can be computed as
    // angle = atan2(vector2.y, vector2.x) - atan2(vector1.y, vector1.x);
    glm::vec3 upFSU(0, 0, 1);
    float rollCorrection = -1.f * (glm::atan(surfaceFSU.y, surfaceFSU.z) - glm::atan(upFSU.y, upFSU.z));

    // get angle to modify pitch to match surface plane
    // pitch is angle between up and front
    glm::vec3 forwardFSU(1, 0, 0);
    float pitchCorrection = -1.f * (glm::atan(upFSU.x, upFSU.z) - glm::atan(surfaceFSU.x, surfaceFSU.z));

#if DEBUG
    std::cout << "Correction (euler): " << rollCorrection / pi * 180.0 << " " << pitchCorrection / pi * 180.0 << std::endl;
#endif
    // add additional roll and pitch to current roll and pitch
    const float rotationSlowDown = 0.2f;
    mRoll += rollCorrection / pi * 180.0 * rotationSlowDown;
    mPitch += pitchCorrection / pi * 180.0 * rotationSlowDown;
}
