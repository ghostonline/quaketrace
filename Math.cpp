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
