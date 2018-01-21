#include "Utils.h"
#include <sstream>

unsigned long Utils::clockTimeMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

glm::quat Utils::quatFromEuler(float roll, float pitch, float yaw)
{
    glm::quat forward(0, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat side(0, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat up(0, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat rotYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(up.x, up.y, up.z));
    forward = rotYaw * forward * glm::inverse(rotYaw);
    side = rotYaw * side * glm::inverse(rotYaw);

    glm::quat rotRoll = glm::angleAxis(glm::radians(roll), glm::vec3(forward.x, forward.y, forward.z));
    side = rotRoll * side * glm::inverse(rotRoll);

    glm::quat rotPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(side.x, side.y, side.z));

    glm::quat result = rotPitch * rotRoll * rotYaw;
    return result;
}

glm::quat Utils::quatFromTwoVectors(glm::vec3 u, glm::vec3 v)
{
    float norm_u_norm_v = sqrt(dot(u, u) * glm::dot(v, v));
    float real_part = norm_u_norm_v + glm::dot(u, v);
    glm::vec3 w;

    if (real_part < 1.e-6f * norm_u_norm_v)
    {
        /* If u and v are exactly opposite, rotate 180 degrees
         * around an arbitrary orthogonal axis. Axis normalisation
         * can happen later, when we normalise the quaternion. */
        real_part = 0.0f;
        if (abs(u.x) > abs(u.z))
        {
            w = glm::vec3(-u.y, u.x, 0.f);
        }
        else
        {
            w = glm::vec3(0.f, -u.z, u.y);
        }
    }
    else
    {
        /* Otherwise, build quaternion the standard way. */
        w = glm::cross(u, v);
    }

    return glm::normalize(glm::quat(real_part, w.x, w.y, w.z));
}

std::ostream& operator<<(std::ostream& os, const glm::vec2& v)
{
    os << "(" << v[0] << " " << v[1] << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    os << "(" << v[0] << " " << v[1] << " " << v[2] << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::quat& q)
{
    os << "(" << q.w << " " << q.x << " " << q.y << " " << q.z << ")";
    return os;
}

std::vector<std::string> Utils::split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim))
    {
        tokens.push_back(item);
    }
    return tokens;
}
