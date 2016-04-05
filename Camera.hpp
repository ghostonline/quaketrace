#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"

struct Camera
{
    math::Vec3f origin;
    math::Vec3f direction;
    math::Vec3f up;
    math::Vec3f right;
    math::Vec2f halfViewAngles;

    float near;
    float far;

    void setPosition(const math::Vec3f& pos, const math::Vec3f& forward, const math::Vec3f& up);
    void setPositionAndFocus(const math::Vec3f& pos, const math::Vec3f& focus, const math::Vec3f& up);
};

inline void Camera::setPositionAndFocus(const math::Vec3f& pos, const math::Vec3f& focus, const math::Vec3f& up)
{
    setPosition(pos, math::normalized(focus - pos), up);
}
