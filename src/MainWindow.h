#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Landscape.h"
#include "Tank.h"

#include <GL/glut.h>
#include "glm/vec2.hpp"
#include <map>

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();

    void reshape(int width, int height);
    void display();
    void handleKeyboard(unsigned char key, int x, int y);
    void handleKeyboardUp(unsigned char key, int x, int y);
    void idle();

private:
    void stepAnimation();
    void applyKeyboardInput();

    long unsigned int mAnimationStart = 0;
    unsigned long mLastStepTimestamp = 0;
    double mProgress = 0.0;

    Landscape mLandscape;
    glm::vec2 mPosition;
    //double mDirection; // 0..360 deg
    double mCameraHeight = 5.0;
    float mCameraAngle = 0.0;

    std::mt19937* mRandomGenerator;

    glm::vec2 mStartPosition = glm::vec2(0.0, 0.0);
    glm::vec2 mTargetPosition = glm::vec2(0.0, 100.0);

    Tank mTank;
    bool mIsTankOnGround = false;

    typedef unsigned char Key;
    typedef bool Pressed;
    std::map<Key, Pressed> mKeyPressState;

    // light from up at infinity
    std::array<GLfloat, 4> light_position =
                    { 0.0, 0.0, 1.0, 0.0 };

    const float pi = 3.1415926536;

    GLuint mGrassTexture;
};

#endif
