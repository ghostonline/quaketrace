#pragma once

#include <cmath>

#define Vec2f Vec2<float>
#define Vec2i Vec2<int>
#define Mat22f Mat22<float>
#define Mat22i Mat22<int>

namespace math
{
    const float PI = 3.1415f;
    const float PI2 = PI * 2;

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
            return { this->x + rhs.x, this->y + rhs.y };
        }

        inline const Vec2 operator-(const Vec2& rhs) const
        {
            return { this->x - rhs.x, this->y - rhs.y };
        }
        
        inline const Vec2 operator*(T factor) const
        {
            return { this->x * factor, this->y * factor };
        }

        inline const Vec2 operator/(T factor) const
        {
            return { this->x / factor, this->y / factor };
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

    template<typename T>
    struct Mat22
    {
        T buffer[4];

        inline Mat22& operator=(const Mat22& rhs)
        {
            if (this != &rhs)
            {
                buffer[0] = rhs.buffer[0];
                buffer[1] = rhs.buffer[1];
                buffer[2] = rhs.buffer[2];
                buffer[3] = rhs.buffer[3];
            }
            return *this;
        }

        inline Mat22& operator+=(const Mat22& rhs)
        {
            buffer[0] += rhs.buffer[0];
            buffer[1] += rhs.buffer[1];
            buffer[2] += rhs.buffer[2];
            buffer[3] += rhs.buffer[3];
            return *this;
        }

        inline Mat22& operator-=(const Mat22& rhs)
        {
            buffer[0] -= rhs.buffer[0];
            buffer[1] -= rhs.buffer[1];
            buffer[2] -= rhs.buffer[2];
            buffer[3] -= rhs.buffer[3];
            return *this;
        }

        inline Mat22& operator*=(const Mat22& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline Mat22& operator/=(const Mat22& rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        inline Mat22& operator*=(T factor)
        {
            buffer[0] *= factor;
            buffer[1] *= factor;
            buffer[2] *= factor;
            buffer[3] *= factor;
            return *this;
        }

        inline Mat22& operator/=(T factor)
        {
            buffer[0] /= factor;
            buffer[1] /= factor;
            buffer[2] /= factor;
            buffer[3] /= factor;
            return *this;
        }

        inline const Mat22 operator*(Mat22 rhs) const
        {
            Mat22 m; m *= rhs; return m;
        }

        inline const Mat22 operator/(Mat22 rhs) const
        {
            Mat22 m; m /= rhs; return m;
        }

        inline Mat22& operator%=(int factor)
        {
            buffer[0] %= factor;
            buffer[1] %= factor;
            buffer[2] %= factor;
            buffer[3] %= factor;
            return *this;
        }

        inline const Mat22 operator+(const Mat22& rhs) const
        {
            Mat22 m; m += rhs; return m;
        }

        inline const Mat22 operator-(const Mat22& rhs) const
        {
            Mat22 m; m -= rhs; return m;
        }

        inline const Mat22 operator*(const Mat22& rhs)
        {
            Mat22 m;
            m.buffer[0] = buffer[0] * rhs.buffer[0] + buffer[1] * rhs.buffer[2];
            m.buffer[1] = buffer[0] * rhs.buffer[1] + buffer[1] * rhs.buffer[3];
            m.buffer[2] = buffer[2] * rhs.buffer[0] + buffer[3] * rhs.buffer[2];
            m.buffer[3] = buffer[2] * rhs.buffer[1] + buffer[3] * rhs.buffer[3];
            return m;
        }

        inline const Mat22 operator/(const Mat22& rhs)
        {
            Mat22 m;
            m.buffer[0] = buffer[0] / rhs.buffer[0] + buffer[1] / rhs.buffer[2];
            m.buffer[1] = buffer[0] / rhs.buffer[1] + buffer[1] / rhs.buffer[3];
            m.buffer[2] = buffer[2] / rhs.buffer[0] + buffer[3] / rhs.buffer[2];
            m.buffer[3] = buffer[2] / rhs.buffer[1] + buffer[3] / rhs.buffer[3];
            return m;
        }

        inline const Mat22 operator*(T factor) const
        {
            Mat22 m; m *= factor; return m;
        }

        inline const Mat22 operator/(T factor) const
        {
            Mat22 m; m /= factor; return m;
        }

        inline const Vec2<T> operator*(const Vec2<T>& rhs)
        {
            Vec2<T> r;
            r.x = buffer[0] * rhs.x + buffer[1] * rhs.y;
            r.y = buffer[2] * rhs.x + buffer[3] * rhs.y;
            return r;
        }

        inline bool operator==(const Mat22& rhs)
        {
            return buffer[0] == rhs.buffer[0]
                && buffer[1] == rhs.buffer[1]
                && buffer[2] == rhs.buffer[2]
                && buffer[3] == rhs.buffer[3];
        }

        inline bool operator!=(const Mat22& rhs)
        {
            return buffer[0] != rhs.buffer[0]
                || buffer[1] != rhs.buffer[1]
                || buffer[2] != rhs.buffer[2]
                || buffer[3] != rhs.buffer[3];
        }
    };

    const Mat22f createRotationMatrix(float angle);
    const Mat22f createCameraMatrix(float planeX, float planeY, float dirX, float dirY);
    const Mat22f invertMatrix(const Mat22f& m);

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
