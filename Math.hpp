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
    const float APPROXIMATE_ZERO = 0.00001f;

    inline const float deg2rad(float deg) { return (PI / 180.0f) * deg; }

    template<typename T>
    inline const T squared(T num) { return num * num; }

    template<typename T>
    inline const T min(T a, T b) { return a < b ? a : b; }

    template<typename T>
    inline const T max(T a, T b) { return a > b ? a : b; }

    template<typename T>
    inline const T clamp(T num, T min, T max) { return math::max(min, math::min(num, max)); }
    inline const float clamp01(float num) { return clamp(num, 0.0f, 1.0f); }

    const Mat22f createRotationMatrix(float angle);
    const Mat22f createCameraMatrix(float planeX, float planeY, float dirX, float dirY);
    const Mat22f invertMatrix(const Mat22f& m);

    const Mat33f createRotationMatrix(const Vec3f& unit, float angle);
}
