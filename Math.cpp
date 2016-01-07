#include "Math.hpp"

const math::Mat22f math::createRotationMatrix(float angle)
{
    math::Mat22f m;
    m.buffer[0] = std::cos(angle);
    m.buffer[1] = -std::sin(angle);
    m.buffer[2] = std::sin(angle);
    m.buffer[3] = std::cos(angle);
    return m;
}

const math::Mat22f math::createCameraMatrix(float planeX, float planeY, float dirX, float dirY)
{
    math::Mat22f m;
    m.buffer[0] = planeX;
    m.buffer[1] = dirX;
    m.buffer[2] = planeY;
    m.buffer[3] = dirY;
    return m;
}

const math::Mat22f math::invertMatrix(const math::Mat22f& m)
{
    math::Mat22f inv;
    inv.buffer[0] = m.buffer[3];
    inv.buffer[3] = m.buffer[0];
    inv.buffer[1] = m.buffer[1] * -1.0f;
    inv.buffer[2] = m.buffer[2] * -1.0f;
    float D = 1.0f / (m.buffer[0] * m.buffer[3] - m.buffer[1] * m.buffer[2]);
    inv *= D;
    return inv;
}

const math::Mat33f math::createRotationMatrix(const Vec3f& u, float angle)
{
    const Mat33f uxu = {
        {u.x * u.x, u.x * u.y, u.x * u.z,
        u.y * u.x, u.y * u.y, u.y * u.z,
        u.z * u.x, u.z * u.y, u.z * u.z},
    };

    const Mat33f ucross = {
        {0.0f, -u.z,  u.y,
         u.z, 0.0f, -u.x,
        -u.y,  u.x, 0.0f},
    };

    static const Mat33f i = {
        {1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f},
    };

    return i * std::cos(angle) + ucross * std::sin(angle) + uxu * (1 - std::cos(angle));
}
