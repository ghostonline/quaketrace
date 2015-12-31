#pragma once

#include <cmath>

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Mat22.hpp"
#include "Mat33.hpp"
#include "Mat44.hpp"

namespace math
{
    const float PI = 3.1415f;
    const float PI2 = PI * 2;

    inline const float deg2rad(float deg) { return (PI / 180.0f) * deg; }

    template<typename T>
    const float squared(T num) { return num * num; }

    const Mat22f createRotationMatrix(float angle);
    const Mat22f createCameraMatrix(float planeX, float planeY, float dirX, float dirY);
    const Mat22f invertMatrix(const Mat22f& m);

    const Mat33f createRotationMatrix(const Vec3f& unit, float angle);
}
