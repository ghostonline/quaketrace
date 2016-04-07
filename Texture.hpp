#pragma once

#include <vector>
#include <cstdint>
#include "Color.hpp"

class Texture
{
public:
    static Texture createFromIndexedRGB(int width, int height, const void* indices, const void* palette);

    Texture(int width, int height, int channels);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getPitch() const { return pitch; }
    std::uint8_t* getPixels() { return pixels.data(); }
    const std::uint8_t* getPixels() const { return pixels.data(); }

    const Color sample(int x, int y) const;

private:
    int width;
    int height;
    int pitch;
    int channels;
    std::vector<std::uint8_t> pixels;
};
