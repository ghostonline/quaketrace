#pragma once

#include <vector>
#include <cstdint>

struct Image
{
    enum Format
    {
        FORMAT_ARGB,
    };

    int width;
    int height;
    std::vector<std::uint8_t> pixels;
    Format format;

    Image(int width, int height, Format format);
    Image(const std::vector<std::uint8_t>& pixels, int width, int height, Format format);
    int getPixelSize() const { return FORMAT_PIXEL_SIZE[format]; }

private:
    static const int FORMAT_PIXEL_SIZE[];
};
