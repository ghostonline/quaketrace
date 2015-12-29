#pragma once

#include <cmath>

#define Vec2f Vec2<float>
#define Vec2i Vec2<int>

namespace math
{

    template<typename T>
    struct Vec2
    {
        T x;
        T y;

        inline Vec2& operator=(const Vec2& rhs)
        {
            if (this != &rhs)
            {
                this->x = rhs.x;
                this->y = rhs.y;
            }
            return *this;
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

    };

    inline const float distance2(const Vec2f& a, const Vec2f& b);
    inline const float distance(const Vec2f& a, const Vec2f& b);
}

const float math::distance2(const Vec2f& a, const Vec2f& b)
{
    auto rel = b - a;
    return rel.x * rel.x + rel.y * rel.y;
}

const float math::distance(const Vec2f& a, const Vec2f& b)
{
    return std::sqrt(distance2(a, b));
}
