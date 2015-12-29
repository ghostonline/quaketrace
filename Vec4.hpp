#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec4
    {
        T x, y, z, w;

        Vec4() : x(0), y(0), z(0), w(1) {}
        Vec4(T x, T y, T z) : x(x), y(y), z(z), w(1) {}
        Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        inline Vec4& operator+=(const Vec4& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            this->w += rhs.w;
            return *this;
        }

        inline Vec4& operator-=(const Vec4& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            this->w -= rhs.w;
            return *this;
        }

        inline Vec4& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            this->w *= factor;
            return *this;
        }

        inline Vec4& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            this->z /= factor;
            this->w /= factor;
            return *this;
        }

        inline const Vec4 operator+(const Vec4& rhs) const
        {
            return
            { this->x + rhs.x
            , this->y + rhs.y
            , this->z + rhs.z
            , this->w + rhs.w
            };
        }

        inline const Vec4 operator-(const Vec4& rhs) const
        {
            return
            { this->x - rhs.x
            , this->y - rhs.y
            , this->z - rhs.z
            , this->w - rhs.w
            };
        }

        inline const Vec4 operator*(T factor) const
        {
            return
            { this->x * factor
            , this->y * factor
            , this->z * factor
            , this->w * factor
            };
        }

        inline const Vec4 operator/(T factor) const
        {
            return
            { this->x / factor
            , this->y / factor
            , this->z / factor
            , this->w / factor
            };
        }

        inline bool operator==(const Vec4& rhs)
        {
            return this->x == rhs.x
                && this->y == rhs.y
                && this->z == rhs.z
                && this->w == rhs.w
                ;
        }

        inline bool operator!=(const Vec4& rhs)
        {
            return this->x != rhs.x
                || this->y != rhs.y
                || this->z != rhs.z
                || this->w != rhs.w
                ;
        }

        inline Vec4& set(T x, T y, T z, T w = 1)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
            return *this;
        }

        inline static const float length2(const Vec4& p)
        {
            return p.x * p.x + p.y * p.y + p.z * p.z + p.w * p.w;
        }

        inline static const float length(const Vec4& p)
        {
            return std::sqrt(length2(p));
        }

        inline static const float distance2(const Vec4& a, const Vec4& b)
        {
            return length2(b - a);
        }

        inline static const float distance(const Vec4& a, const Vec4& b)
        {
            return length(b - a);
        }

        inline static void normalize(Vec4* v)
        {
            *v /= length(*v);
        }

        inline static float dot(const Vec4& a, const Vec4& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }
    };

    typedef Vec4<float> Vec4f;
    typedef Vec4<int> Vec4i;
}
