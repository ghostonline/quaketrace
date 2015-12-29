#pragma once

#include <cmath>

#include "Vec2.hpp"
#include "Mat22.hpp"

namespace math
{
    const float PI = 3.1415f;
    const float PI2 = PI * 2;

    const Mat22f createRotationMatrix(float angle);
    const Mat22f createCameraMatrix(float planeX, float planeY, float dirX, float dirY);
    const Mat22f invertMatrix(const Mat22f& m);
}
