#pragma once

#include <cstdint>
#include "Math.hpp"

struct Color
{
    Color(float v) : r(v), g(v), b(v) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    float r, g, b;

    inline static std::uint32_t asUint(const Color& c)
    {
        return float2byte(c.r, 16)
            + float2byte(c.g, 8)
            + float2byte(c.b, 0)
            + 0xFF000000;
    }

    inline Color& operator+=(const Color& rhs)
    {
        this->r += rhs.r;
        this->g += rhs.g;
        this->b += rhs.b;
        return *this;
    }

    inline const Color operator+(const Color& rhs) const
    {
        Color c = *this;
        c += rhs;
        return c;
    }

    inline Color& operator-=(const Color& rhs)
    {
        this->r = rhs.r;
        this->g = rhs.g;
        this->b = rhs.b;
        return *this;
    }

    inline const Color operator-(const Color& rhs) const
    {
        Color c = *this;
        c -= rhs;
        return c;
    }

    inline Color& operator*=(float factor)
    {
        this->r *= factor;
        this->g *= factor;
        this->b *= factor;
        return *this;
    }

    inline const Color operator*(float factor) const
    {
        Color c = *this;
        c *= factor;
        return c;
    }

    inline Color& operator/=(float factor)
    {
        this->r /= factor;
        this->g /= factor;
        this->b /= factor;
        return *this;
    }

    inline const Color operator/(float factor) const
    {
        Color c = *this;
        c /= factor;
        return c;
    }

    inline static const Color blendLinear(const Color& a, const Color& b, float factor)
    {
        return
        {
            a.r * factor + b.r * (1.0f - factor),
            a.b * factor + b.b * (1.0f - factor),
            a.g * factor + b.g * (1.0f - factor),
        };
    }

private:
    static inline const std::uint32_t float2byte(float v, int lshift = 0) { return (static_cast<std::uint32_t>(v * 0xFF) & 0xFF) << lshift; }
};
