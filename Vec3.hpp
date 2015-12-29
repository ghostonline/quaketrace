#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3() : x(0), y(0), z(0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

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

        inline Vec3& set(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            return *this;
        }

        inline static const float length2(const Vec3& p)
        {
            return p.x * p.x + p.y * p.y + p.z * p.z;
        }

        inline static const float length(const Vec3& p)
        {
            return std::sqrt(length2(p));
        }

        inline static const float distance2(const Vec3& a, const Vec3& b)
        {
            return length2(b - a);
        }

        inline static const float distance(const Vec3& a, const Vec3& b)
        {
            return length(b - a);
        }

    };

    typedef Vec3<float> Vec3f;
    typedef Vec3<int> Vec3i;
}
