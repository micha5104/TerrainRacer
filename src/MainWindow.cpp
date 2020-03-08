#include "MainWindow.h"
#include "Constants.h"
#include "Utils.h"
#include "Matrix.h"
#include "glm/mat3x3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils/Jpeg.h"

#include <iostream>

constexpr auto EPSILON = (1e-8);

#define DEBUG 0

MainWindow::MainWindow()
{
    mRandomGenerator = new std::mt19937();

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	// enable next line to only draw wireframes
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    glLightfv(GL_LIGHT0, GL_POSITION, &light_position[0]);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    mLandscape.generate(Landscape::Type::FILE);

    Jpeg jpeg;
    jpeg.load("images/Textures/grass.jpg");

    glGenTextures(1, &mGrassTexture);
    glBindTexture(GL_TEXTURE_2D, mGrassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, jpeg.width(), jpeg.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, jpeg.data());

    mTank.setPosition(glm::vec3(0, 0, 0));

    mAnimationStart = Utils::clockTimeMs();
}

MainWindow::~MainWindow()
{
    delete mRandomGenerator;
}

void MainWindow::reshape(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 1010.0);
    glMatrixMode(GL_MODELVIEW);
}

void MainWindow::display()
{
    mFrame++;
    unsigned long start = Utils::clockTimeMs();
    //std::cout << "display" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, &light_position[0]);
    //glColor3f(0.0f, 1.0, 0.0f);

    // calculate new position of tank
    glm::vec3 newPos = mTank.move();
    mPosition.x = newPos.x;
    mPosition.y = newPos.y;

    // get landscape at new position
    float height = 0;
    glm::vec3 surfaceNormal;
    mLandscape.getLocalEnvironment(mPosition.x, mPosition.y, height, surfaceNormal);

    // adjust tank position
    if (height > newPos.z)
    {
        newPos.z = std::max(height, newPos.z);
        mTank.setPosition(newPos);
    }

    // adjust tank orientation according to surface normal if tank touches the ground
    mIsTankOnGround = (fabs(height - newPos.z) < EPSILON);
    if (mIsTankOnGround)
    {
        mTank.rotateToMatchSurfaceNormal(surfaceNormal);
    }

    // camera position
    glm::vec3 tankToCamera(-10.0, 0.0, mCameraHeight);
    glm::quat rot = glm::angleAxis((mTank.yaw() + mCameraAngle) / 180.0f * pi / 2.0f, glm::vec3(0, 0, 1));
    tankToCamera = rot * tankToCamera * glm::conjugate(rot);
    glm::vec3 cameraPos = mTank.position();
    cameraPos += tankToCamera;

    glm::vec3 eye(cameraPos.x, cameraPos.y, cameraPos.z);
    glm::vec3 center(mPosition.x, mPosition.y, mTank.position().z);
    glm::vec3 up(0, 0, 1);

#if DEBUG
    std::cout << "MainWindow" << std::endl;
    std::cout << "\tEye:    " << eye.x << " " << eye.y << " " << eye.z << std::endl;
    std::cout << "\tCenter: " << center.x << " " << center.y << " " << center.z << std::endl;
    std::cout << "\tUp: " << up.x << " " << up.y << " " << up.z << std::endl;
#endif
    glm::mat4 lookAt = glm::lookAt(eye, center, up);
    glLoadMatrixf(glm::value_ptr(lookAt));
    //gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mGrassTexture);
    mLandscape.draw(mPosition, 100);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    mLandscape.drawNormals(mPosition, 0);
    mTank.draw();

    unsigned long end = Utils::clockTimeMs();
    if (mFrame % 100 == 0)
    {
        std::cout << "FPS: " << 1000.0 / (end - start) << std::endl;
    }
    glFlush();
}

void MainWindow::handleKeyboard(unsigned char key, int x, int y)
{
    mKeyPressState[key] = true;
}

void MainWindow::handleKeyboardUp(unsigned char key, int x, int y)
{
    mKeyPressState[key] = false;
    std::cout << "up: " << key << std::endl;
}

void MainWindow::idle()
{
    const unsigned long now = Utils::clockTimeMs();
    if (now - mLastStepTimestamp > 16)
    {
        //std::cout << "step: " << mTank.position() << std::endl;
        applyKeyboardInput();
        glutPostRedisplay();
        mLastStepTimestamp = now;
    }
}

void MainWindow::applyKeyboardInput()
{
    const double accelerationStep = 0.005;
    const double rotationStep = 0.6;
    const int stepSize = 2.0;
    for (const auto& entry : mKeyPressState)
    {
        if (entry.second)
        {
            if (entry.first == 'a' && mIsTankOnGround)
            {
                //std::cout << "left" << std::endl;
                float yaw = mTank.yaw();
                yaw += rotationStep;
                mTank.setYaw(yaw);
            }
            else if (entry.first == 'A' && mIsTankOnGround)
            {
                float yaw = mTank.yaw();
                yaw += rotationStep / 10.0f;
                mTank.setYaw(yaw);
            }
            else if (entry.first == 'd' && mIsTankOnGround)
            {
                //std::cout << "right" << std::endl;
                float yaw = mTank.yaw();
                yaw -= rotationStep;
                mTank.setYaw(yaw);
            }
            else if (entry.first == 'D' && mIsTankOnGround)
            {
                //std::cout << "right" << std::endl;
                float yaw = mTank.yaw();
                yaw -= rotationStep / 10.0f;
                mTank.setYaw(yaw);
            }
            else if (entry.first == 'w' && mIsTankOnGround)
            {
                //std::cout << "speed up" << std::endl;
                mTank.accelerate(accelerationStep);
            }
            else if (entry.first == 'W' && mIsTankOnGround)
            {
                //std::cout << "speed up" << std::endl;
                mTank.accelerate(accelerationStep / 10.0f);
            }
            else if (entry.first == 's' && mIsTankOnGround)
            {
                //std::cout << "slow down" << std::endl;
                mTank.accelerate(-accelerationStep);
            }
            else if (entry.first == 'S' && mIsTankOnGround)
            {
                //std::cout << "slow down" << std::endl;
                mTank.accelerate(-accelerationStep / 10.0f);
            }
            else if (entry.first == 'j') // roll left
            {
                //std::cout << "roll left" << std::endl;
                float roll = mTank.roll();
                roll -= rotationStep;
                mTank.setRoll(roll);
            }
            else if (entry.first == 'l') // roll right
            {
                //std::cout << "roll right" << std::endl;
                float roll = mTank.roll();
                roll += rotationStep;
                mTank.setRoll(roll);
            }
            else if (entry.first == 'i') // pitch up
            {
                //std::cout << "pitch up" << std::endl;
                float pitch = mTank.pitch();
                pitch += rotationStep;
                mTank.setPitch(pitch);
            }
            else if (entry.first == 'k') // pitch down
            {
                //std::cout << "pitch down" << std::endl;
                float pitch = mTank.pitch();
                pitch -= rotationStep;
                mTank.setPitch(pitch);
            }
            else if (entry.first == 'u') // rotate camera
            {
                mCameraAngle += 1.0f;
            }
            else if (entry.first == 'o') // rotate camera
            {
                mCameraAngle -= 1.0f;
            }
            else if (entry.first == '+')
            {
                //std::cout << "camera up" << std::endl;
                mCameraHeight += 1.0;
            }
            else if (entry.first == '-')
            {
                //std::cout << "camera down" << std::endl;
                mCameraHeight -= 1.0;
            }
            else if (entry.first == ' ')
            {
                mTank.stop();
            }
            else if (entry.first == '0')
            {
                mTank.setPitch(0.f);
                mTank.setRoll(0.f);
                mTank.setYaw(0.f);
            }
        }
    }
}
