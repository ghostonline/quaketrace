#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec2
    {
        T x, y;

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

    };

    typedef Vec2<float> Vec2f;
    typedef Vec2<int> Vec2i;
}
