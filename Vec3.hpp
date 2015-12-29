#pragma once

#include <cmath>

#define Vec3f Vec3<float>
#define Vec3i Vec3<int>

namespace math
{
    template<typename T>
    struct Vec3
    {
        T x;
        T y;
        T z;

        inline Vec3& operator+=(const Vec3& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            return *this;
        }

        inline Vec3& operator-=(const Vec3& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            return *this;
        }

        inline Vec3& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            return *this;
        }

        inline Vec3& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            this->z /= factor;
            return *this;
        }

        inline const Vec3 operator+(const Vec3& rhs) const
        {
            return
            { this->x + rhs.x
            , this->y + rhs.y
            , this->z + rhs.z
            };
        }

        inline const Vec3 operator-(const Vec3& rhs) const
        {
            return
            { this->x - rhs.x
            , this->y - rhs.y
            , this->z - rhs.z
            };
        }

        inline const Vec3 operator*(T factor) const
        {
            return
            { this->x * factor
            , this->y * factor
            , this->z * factor
            };
        }

        inline const Vec3 operator/(T factor) const
        {
            return
            { this->x / factor
            , this->y / factor
            , this->z / factor
            };
        }

        inline bool operator==(const Vec3& rhs)
        {
            return this->x == rhs.x
                && this->y == rhs.y
                && this->z == rhs.z;
        }

        inline bool operator!=(const Vec3& rhs)
        {
            return this->x != rhs.x
                || this->y != rhs.y
                || this->z != rhs.z;
        }
    };

    inline const float distance2(const Vec3f& a, const Vec3f& b);
    inline const float distance(const Vec3f& a, const Vec3f& b);
}

const float math::distance2(const Vec3f& a, const Vec3f& b)
{
    auto rel = b - a;
    return rel.x * rel.x + rel.y * rel.y + rel.z * rel.z;
}

const float math::distance(const Vec3f& a, const Vec3f& b)
{
    return std::sqrt(distance2(a, b));
}
