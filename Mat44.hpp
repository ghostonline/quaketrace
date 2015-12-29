#pragma once

namespace math
{
    template<typename T>
    struct Mat44
    {
        T buffer[16];

        inline Mat44& operator+=(const Mat44& rhs)
        {
            buffer[0] += rhs.buffer[0];
            buffer[1] += rhs.buffer[1];
            buffer[2] += rhs.buffer[2];
            buffer[3] += rhs.buffer[3];
            buffer[4] += rhs.buffer[4];
            buffer[5] += rhs.buffer[5];
            buffer[6] += rhs.buffer[6];
            buffer[7] += rhs.buffer[7];
            buffer[8] += rhs.buffer[8];
            buffer[9] += rhs.buffer[9];
            buffer[10] += rhs.buffer[10];
            buffer[11] += rhs.buffer[11];
            buffer[12] += rhs.buffer[12];
            buffer[13] += rhs.buffer[13];
            buffer[14] += rhs.buffer[14];
            buffer[15] += rhs.buffer[15];
            return *this;
        }

        inline Mat44& operator-=(const Mat44& rhs)
        {
            buffer[0] -= rhs.buffer[0];
            buffer[1] -= rhs.buffer[1];
            buffer[2] -= rhs.buffer[2];
            buffer[3] -= rhs.buffer[3];
            buffer[4] -= rhs.buffer[4];
            buffer[5] -= rhs.buffer[5];
            buffer[6] -= rhs.buffer[6];
            buffer[7] -= rhs.buffer[7];
            buffer[8] -= rhs.buffer[8];
            buffer[9] -= rhs.buffer[9];
            buffer[10] -= rhs.buffer[10];
            buffer[11] -= rhs.buffer[11];
            buffer[12] -= rhs.buffer[12];
            buffer[13] -= rhs.buffer[13];
            buffer[14] -= rhs.buffer[14];
            buffer[15] -= rhs.buffer[15];
            return *this;
        }

        inline Mat44& operator*=(const Mat44& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline Mat44& operator/=(const Mat44& rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        inline Mat44& operator*=(T factor)
        {
            buffer[0] *= factor;
            buffer[1] *= factor;
            buffer[2] *= factor;
            buffer[3] *= factor;
            buffer[4] *= factor;
            buffer[5] *= factor;
            buffer[6] *= factor;
            buffer[7] *= factor;
            buffer[8] *= factor;
            buffer[9] *= factor;
            buffer[10] *= factor;
            buffer[11] *= factor;
            buffer[12] *= factor;
            buffer[13] *= factor;
            buffer[14] *= factor;
            buffer[15] *= factor;
            return *this;
        }

        inline Mat44& operator/=(T factor)
        {
            buffer[0] /= factor;
            buffer[1] /= factor;
            buffer[2] /= factor;
            buffer[3] /= factor;
            buffer[4] /= factor;
            buffer[5] /= factor;
            buffer[6] /= factor;
            buffer[7] /= factor;
            buffer[8] /= factor;
            buffer[9] /= factor;
            buffer[10] /= factor;
            buffer[11] /= factor;
            buffer[12] /= factor;
            buffer[13] /= factor;
            buffer[14] /= factor;
            buffer[15] /= factor;
            return *this;
        }

        inline const Mat44 operator*(Mat44 rhs) const
        {
            Mat44 m; m *= rhs; return m;
        }

        inline const Mat44 operator/(Mat44 rhs) const
        {
            Mat44 m; m /= rhs; return m;
        }

        inline Mat44& operator%=(int factor)
        {
            buffer[0] %= factor;
            buffer[1] %= factor;
            buffer[2] %= factor;
            buffer[3] %= factor;
            buffer[4] %= factor;
            buffer[5] %= factor;
            buffer[6] %= factor;
            buffer[7] %= factor;
            buffer[8] %= factor;
            buffer[9] %= factor;
            buffer[10] %= factor;
            buffer[11] %= factor;
            buffer[12] %= factor;
            buffer[13] %= factor;
            buffer[14] %= factor;
            buffer[15] %= factor;
            return *this;
        }

        inline const Mat44 operator+(const Mat44& rhs) const
        {
            Mat44 m; m += rhs; return m;
        }

        inline const Mat44 operator-(const Mat44& rhs) const
        {
            Mat44 m; m -= rhs; return m;
        }

        inline const Mat44 operator*(const Mat44& rhs)
        {
            // TODO
            Mat44 m;
            m.buffer[0] = buffer[0] * rhs.buffer[0] + buffer[1] * rhs.buffer[4] + buffer[2] * rhs.buffer[8] + buffer[3] * rhs.buffer[12];
            m.buffer[1] = buffer[0] * rhs.buffer[1] + buffer[1] * rhs.buffer[5] + buffer[2] * rhs.buffer[9] + buffer[3] * rhs.buffer[13];
            m.buffer[2] = buffer[0] * rhs.buffer[2] + buffer[1] * rhs.buffer[6] + buffer[2] * rhs.buffer[10] + buffer[3] * rhs.buffer[14];
            m.buffer[3] = buffer[0] * rhs.buffer[3] + buffer[1] * rhs.buffer[7] + buffer[2] * rhs.buffer[11] + buffer[3] * rhs.buffer[15];
            m.buffer[4] = buffer[4] * rhs.buffer[0] + buffer[5] * rhs.buffer[4] + buffer[6] * rhs.buffer[8] + buffer[7] * rhs.buffer[12];
            m.buffer[5] = buffer[4] * rhs.buffer[1] + buffer[5] * rhs.buffer[5] + buffer[6] * rhs.buffer[9] + buffer[7] * rhs.buffer[13];
            m.buffer[6] = buffer[4] * rhs.buffer[2] + buffer[5] * rhs.buffer[6] + buffer[6] * rhs.buffer[10] + buffer[7] * rhs.buffer[14];
            m.buffer[7] = buffer[4] * rhs.buffer[3] + buffer[5] * rhs.buffer[7] + buffer[6] * rhs.buffer[11] + buffer[7] * rhs.buffer[15];
            m.buffer[8] = buffer[8] * rhs.buffer[0] + buffer[9] * rhs.buffer[4] + buffer[10] * rhs.buffer[8] + buffer[11] * rhs.buffer[12];
            m.buffer[9] = buffer[8] * rhs.buffer[1] + buffer[9] * rhs.buffer[5] + buffer[10] * rhs.buffer[9] + buffer[11] * rhs.buffer[13];
            m.buffer[10] = buffer[8] * rhs.buffer[2] + buffer[9] * rhs.buffer[6] + buffer[10] * rhs.buffer[10] + buffer[11] * rhs.buffer[14];
            m.buffer[11] = buffer[8] * rhs.buffer[3] + buffer[9] * rhs.buffer[7] + buffer[10] * rhs.buffer[11] + buffer[11] * rhs.buffer[15];
            m.buffer[12] = buffer[12] * rhs.buffer[0] + buffer[13] * rhs.buffer[4] + buffer[14] * rhs.buffer[8] + buffer[15] * rhs.buffer[12];
            m.buffer[13] = buffer[12] * rhs.buffer[1] + buffer[13] * rhs.buffer[5] + buffer[14] * rhs.buffer[9] + buffer[15] * rhs.buffer[13];
            m.buffer[14] = buffer[12] * rhs.buffer[2] + buffer[13] * rhs.buffer[6] + buffer[14] * rhs.buffer[10] + buffer[15] * rhs.buffer[14];
            m.buffer[15] = buffer[12] * rhs.buffer[3] + buffer[13] * rhs.buffer[7] + buffer[14] * rhs.buffer[11] + buffer[15] * rhs.buffer[15];
            return m;
        }

        inline const Mat44 operator/(const Mat44& rhs)
        {
            Mat44 m;
            m.buffer[0] = buffer[0] / rhs.buffer[0] + buffer[1] / rhs.buffer[4] + buffer[2] / rhs.buffer[8] + buffer[3] / rhs.buffer[12];
            m.buffer[1] = buffer[0] / rhs.buffer[1] + buffer[1] / rhs.buffer[5] + buffer[2] / rhs.buffer[9] + buffer[3] / rhs.buffer[13];
            m.buffer[2] = buffer[0] / rhs.buffer[2] + buffer[1] / rhs.buffer[6] + buffer[2] / rhs.buffer[10] + buffer[3] / rhs.buffer[14];
            m.buffer[3] = buffer[0] / rhs.buffer[3] + buffer[1] / rhs.buffer[7] + buffer[2] / rhs.buffer[11] + buffer[3] / rhs.buffer[15];
            m.buffer[4] = buffer[4] / rhs.buffer[0] + buffer[5] / rhs.buffer[4] + buffer[6] / rhs.buffer[8] + buffer[7] / rhs.buffer[12];
            m.buffer[5] = buffer[4] / rhs.buffer[1] + buffer[5] / rhs.buffer[5] + buffer[6] / rhs.buffer[9] + buffer[7] / rhs.buffer[13];
            m.buffer[6] = buffer[4] / rhs.buffer[2] + buffer[5] / rhs.buffer[6] + buffer[6] / rhs.buffer[10] + buffer[7] / rhs.buffer[14];
            m.buffer[7] = buffer[4] / rhs.buffer[3] + buffer[5] / rhs.buffer[7] + buffer[6] / rhs.buffer[11] + buffer[7] / rhs.buffer[15];
            m.buffer[8] = buffer[8] / rhs.buffer[0] + buffer[9] / rhs.buffer[4] + buffer[10] / rhs.buffer[8] + buffer[11] / rhs.buffer[12];
            m.buffer[9] = buffer[8] / rhs.buffer[1] + buffer[9] / rhs.buffer[5] + buffer[10] / rhs.buffer[9] + buffer[11] / rhs.buffer[13];
            m.buffer[10] = buffer[8] / rhs.buffer[2] + buffer[9] / rhs.buffer[6] + buffer[10] / rhs.buffer[10] + buffer[11] / rhs.buffer[14];
            m.buffer[11] = buffer[8] / rhs.buffer[3] + buffer[9] / rhs.buffer[7] + buffer[10] / rhs.buffer[11] + buffer[11] / rhs.buffer[15];
            m.buffer[12] = buffer[12] / rhs.buffer[0] + buffer[13] / rhs.buffer[4] + buffer[14] / rhs.buffer[8] + buffer[15] / rhs.buffer[12];
            m.buffer[13] = buffer[12] / rhs.buffer[1] + buffer[13] / rhs.buffer[5] + buffer[14] / rhs.buffer[9] + buffer[15] / rhs.buffer[13];
            m.buffer[14] = buffer[12] / rhs.buffer[2] + buffer[13] / rhs.buffer[6] + buffer[14] / rhs.buffer[10] + buffer[15] / rhs.buffer[14];
            m.buffer[15] = buffer[12] / rhs.buffer[3] + buffer[13] / rhs.buffer[7] + buffer[14] / rhs.buffer[11] + buffer[15] / rhs.buffer[15];
            return m;
        }

        inline const Mat44 operator*(T factor) const
        {
            Mat44 m; m *= factor; return m;
        }

        inline const Mat44 operator/(T factor) const
        {
            Mat44 m; m /= factor; return m;
        }

        inline const Vec4<T> operator*(const Vec4<T>& rhs)
        {
            Vec4<T> r;
            r.x = buffer[0] * rhs.x + buffer[1] * rhs.y + buffer[2] * rhs.z + buffer[3] * rhs.w;
            r.y = buffer[4] * rhs.x + buffer[5] * rhs.y + buffer[6] * rhs.z + buffer[7] * rhs.w;
            r.z = buffer[8] * rhs.x + buffer[9] * rhs.y + buffer[10] * rhs.z + buffer[11] * rhs.w;
            r.w = buffer[12] * rhs.x + buffer[13] * rhs.y + buffer[14] * rhs.z + buffer[15] * rhs.w;
            return r;
        }

        inline bool operator==(const Mat44& rhs)
        {
            return buffer[0] == rhs.buffer[0]
                && buffer[1] == rhs.buffer[1]
                && buffer[2] == rhs.buffer[2]
                && buffer[3] == rhs.buffer[3]
                && buffer[4] == rhs.buffer[4]
                && buffer[5] == rhs.buffer[5]
                && buffer[6] == rhs.buffer[6]
                && buffer[7] == rhs.buffer[7]
                && buffer[8] == rhs.buffer[8]
                && buffer[9] == rhs.buffer[9]
                && buffer[10] == rhs.buffer[10]
                && buffer[11] == rhs.buffer[11]
                && buffer[12] == rhs.buffer[12]
                && buffer[13] == rhs.buffer[13]
                && buffer[14] == rhs.buffer[14]
                && buffer[15] == rhs.buffer[15]
                ;
        }

        inline bool operator!=(const Mat44& rhs)
        {
            return buffer[0] != rhs.buffer[0]
                || buffer[1] != rhs.buffer[1]
                || buffer[2] != rhs.buffer[2]
                || buffer[3] != rhs.buffer[3]
                || buffer[4] != rhs.buffer[4]
                || buffer[5] != rhs.buffer[5]
                || buffer[6] != rhs.buffer[6]
                || buffer[7] != rhs.buffer[7]
                || buffer[8] != rhs.buffer[8]
                || buffer[9] != rhs.buffer[9]
                || buffer[10] != rhs.buffer[10]
                || buffer[11] != rhs.buffer[11]
                || buffer[12] != rhs.buffer[12]
                || buffer[13] != rhs.buffer[13]
                || buffer[14] != rhs.buffer[14]
                || buffer[15] != rhs.buffer[15]
                ;
        }
    };

    typedef Mat44<float> Mat44f;
    typedef Mat44<int> Mat44i;
}
