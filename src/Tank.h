#ifndef TANK_H
#define TANK_H

#include "glm/vec3.hpp"
#include <vector>

class Tank
{
public:
    Tank();
    ~Tank() = default;

    // move the tank according to its position speed orientation, ...
    // returns the new position
    glm::vec3 move();

    // draw the tank at mPosition
    void draw();

    void setOrientation(float pitch, float yaw, float roll);
    void setRoll(float roll);
    void setPitch(float pitch);
    void setYaw(float yaw);

    void rotateToMatchSurfaceNormal(const glm::vec3& surfaceNormal);

    /* @brief the orientation of the tank.
     * These are angles in degrees: roll (around x-axis), pitch (around y-axis), yaw (around z-axis)
     */
    float roll() const;
    float pitch() const;
    float yaw() const;

    void setPosition(glm::vec3 position);
    glm::vec3 position() const;

    void accelerate(double acceleration);
    double velocity() const;
    void stop();

private:
    enum class Model
    {
        Cube,
        Simple,
        File
    };
    /* @brief just draw the model; no positioning transformations.
     */
    void drawModel(Model model);

    // x, y, z
    glm::vec3 mPosition;

    // these are angles in degrees: roll (around x-axis), pitch (around y-axis), yaw (around z-axis);
    float mPitch = 0.0f;
    float mYaw = 0.0f;
    float mRoll = 0.0f;

    float mVelocity = 0.0f;

    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUvs;
    bool mModelLoaded = false;

    const float pi = 3.1415926536f;
};

#endif
