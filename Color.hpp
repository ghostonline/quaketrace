#pragma once

#include <cstdint>

struct Color
{
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    float r, g, b;

    inline static std::uint32_t asUint(const Color& c)
    {
        return static_cast<std::uint32_t>(c.r * 0x00FF0000 + c.g * 0x0000FF00 + c.b * 0x000000FF) + 0xFF000000;
    }
};
