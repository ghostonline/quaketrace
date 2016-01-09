#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec2
    {
        T x, y;

        Vec2() : x(0), y(0){}
        Vec2(T x, T y) : x(x), y(y) {}

        inline Vec2 operator-() const
        {
            return{
                -x,
                -y,
            };
        }

        inline Vec2& operator+=(const Vec2& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        inline Vec2& operator-=(const Vec2& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }

        inline Vec2& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            return *this;
        }

        inline Vec2& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            return *this;
        }

        inline const Vec2 operator+(const Vec2& rhs) const
        {
            return{ this->x + rhs.x, this->y + rhs.y };
        }

        inline const Vec2 operator-(const Vec2& rhs) const
        {
            return{ this->x - rhs.x, this->y - rhs.y };
        }

        inline const Vec2 operator*(T factor) const
        {
            return{ this->x * factor, this->y * factor };
        }

        inline const Vec2 operator/(T factor) const
        {
            return{ this->x / factor, this->y / factor };
        }

        inline bool operator==(const Vec2& rhs)
        {
            return this->x == rhs.x && this->y == rhs.y;
        }

        inline bool operator!=(const Vec2& rhs)
        {
            return this->x != rhs.x || this->y != rhs.y;
        }

        inline Vec2& set(T x, T y)
        {
            this->x = x;
            this->y = y;
            return *this;
        }

        inline static const float length2(const Vec2& p)
        {
            return p.x * p.x + p.y * p.y;
        }

        inline static const float length(const Vec2& p)
        {
            return std::sqrt(length2(p));
        }

        inline static const float distance2(const Vec2& a, const Vec2& b)
        {
            return length2(b - a);
        }

        inline static const float distance(const Vec2& a, const Vec2& b)
        {
            return length(b - a);
        }

        inline static void normalize(Vec2& v)
        {
            *v /= length(v);
        }

        inline static const Vec2 normalized(const Vec2& v)
        {
            Vec2 vec = v;
            normalize(&vec);
            return vec;
        }

        inline static float dot(const Vec2& a, const Vec2& b)
        {
            return a.x * b.x + a.y * b.y;
        }

        inline static float rotate(Vec2* v, float angle)
        {
            v->x = std::cos(angle) * v->x - std::sin(angle) * v->y;
            v->y = std::sin(angle) * v->x + std::cos(angle) * v->y;
        }
    };

    typedef Vec2<float> Vec2f;
    typedef Vec2<int> Vec2i;
}
