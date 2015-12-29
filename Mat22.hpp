#define Mat22f Mat22<float>
#define Mat22i Mat22<int>

namespace math
{
    template<typename T>
    struct Mat22
    {
        T buffer[4];

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
}
