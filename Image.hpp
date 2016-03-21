#pragma once

#include <vector>
#include <cstdint>

struct Image
{
    enum Format
    {
        FORMAT_ARGB,
        NUM_FORMATS,
    };

    enum Channel
    {
        RED,
        GREEN,
        BLUE,
        ALPHA,
        NUM_CHANNELS,
    };

    int width;
    int height;
    std::vector<std::uint8_t> pixels;
    Format format;

    Image(int width, int height, Format format);
    Image(const std::vector<std::uint8_t>& pixels, int width, int height, Format format);
    int getPixelSize() const { return FORMAT_PIXEL_SIZE[format]; }
    int getChannelOffset(Channel channel) const { return CHANNEL_OFFSET_LOOKUP[format].channels[channel]; }

private:
    union Offset
    {
        struct
        {
            std::uint8_t r;
            std::uint8_t g;
            std::uint8_t b;
            std::uint8_t a;
        };
        std::uint8_t channels[4];
    };

    static const int FORMAT_PIXEL_SIZE[];
    static const Offset CHANNEL_OFFSET_LOOKUP[];
};
