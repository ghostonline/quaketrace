#pragma once

#include <cstdint>
#include "Math.hpp"
#include <cmath>

struct Color
{
    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    explicit Color(float v) : r(v), g(v), b(v) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    float r, g, b;

    inline static std::uint32_t asARGB(const Color& c)
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
        this->r -= rhs.r;
        this->g -= rhs.g;
        this->b -= rhs.b;
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
            a.g * factor + b.g * (1.0f - factor),
            a.b * factor + b.b * (1.0f - factor),
        };
    }

    inline static void normalize(Color* c)
    {
        c->r = math::clamp01(c->r);
        c->g = math::clamp01(c->g);
        c->b = math::clamp01(c->b);
    }

    inline static bool isBlack(const Color& c)
    {
        return c.r == 0.0f && c.g == 0.0f && c.b == 0.0f;
    }

    inline static void modifyGamma(Color* c, float gamma)
    {
        c->r = std::powf(c->r, 1.0f / gamma);
        c->g = std::powf(c->g, 1.0f / gamma);
        c->b = std::powf(c->b, 1.0f / gamma);
    }

private:
    static inline const std::uint32_t float2byte(float v, int lshift = 0) { return (static_cast<std::uint32_t>(v * 0xFF) & 0xFF) << lshift; }
};
