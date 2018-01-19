#ifndef UTILS_H
#define UTILS_H

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>
#include <chrono>

/*
 * Interesting things for later:
 * - Wikipedia: Quaternions and spacial rotation
 *      https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
 * - OpenGL Tutorial 17 : Rotations
 *      http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/#i-need-an-equivalent-of-glulookat-how-do-i-orient-an-object-towards-a-point-
 * - Blog: Quaternion from two vectors: the final version – Lol Engine:
 *      http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final
 * - Multi-textured Terrain in OpenGL3D Game Engine Programming
 *      https://www.3dgep.com/multi-textured-terrain-in-opengl/
 * - Roll, pitch, yaw from 3 planar points
 *      https://de.mathworks.com/matlabcentral/answers/298940-how-to-calculate-roll-pitch-and-yaw-from-xyz-coordinates-of-3-planar-points?requestedDomain=www.mathworks.com
 */
class Utils
{
public:
    Utils() = default;
    ~Utils() = default;

    static unsigned long clockTimeMs();

    // rotation order is yaw (around z-axis), roll (around x-axis), pitch (around y-axis)
    static glm::quat quatFromEuler(float roll, float pitch, float yaw);

    /* @brief Build a unit quaternion representing the rotation
     * from u to v. The input vectors need to be normalised. */
    static glm::quat quatFromTwoVectors(glm::vec3 u, glm::vec3 v);
};

std::ostream& operator<<(std::ostream&, const glm::vec2&);
std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::quat&);

#endif
