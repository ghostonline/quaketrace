#include "Camera.hpp"
#include "Assert.hpp"

void Camera::setPosition(const math::Vec3f& pos, const math::Vec3f& forward, const math::Vec3f& up)
{
    ASSERT(up != forward);
    
    origin = pos;
    direction = forward;
    this->up = up; // General up direction
    right = math::cross(direction, up);
    this->up = math::cross(right, direction);

    math::normalize(&direction);
    math::normalize(&this->up);
    math::normalize(&right);

    near = 10.0f;
    far = 10000.0f;
}
