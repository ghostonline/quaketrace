#include "Texture.hpp"
#include <cstdint>

Texture Texture::createFromIndexedRGB(int width, int height, const void *indices, const void *palette)
{
    static const int PIXEL_WIDTH = 4;
    static const int PALETTE_WIDTH = 3;
    Texture texture(width, height, PIXEL_WIDTH);
    
    auto paletteBytes = reinterpret_cast<const std::uint8_t*>(palette);
    auto indicesBytes = reinterpret_cast<const std::uint8_t*>(indices);

    for (int ii = 0; ii < width * height; ++ii)
    {
        int paletteIdx = indicesBytes[ii] * PALETTE_WIDTH;
        int pixelIdx = ii * PIXEL_WIDTH;

        texture.pixels[pixelIdx + 0] = paletteBytes[paletteIdx + 2];
        texture.pixels[pixelIdx + 1] = paletteBytes[paletteIdx + 1];
        texture.pixels[pixelIdx + 2] = paletteBytes[paletteIdx + 0];
        texture.pixels[pixelIdx + 3] = 0xFF;
    }

    return texture;
}

Texture Texture::createCheckerBoard(int width, int height, int checkerSize, const Color& a, const Color& b)
{
    static const int PIXEL_WIDTH = 4;
    const std::uint32_t argbA = Color::asARGB(a);
    const std::uint32_t argbB = Color::asARGB(b);

    Texture texture(width, height, PIXEL_WIDTH);
    int cellX = 0;
    int cellY = 0;
    for (int x = 0; x < width; ++x)
    {
        cellY = 0;
        for (int y = 0; y < height; ++y)
        {
            const std::uint32_t argb = (cellX & 1) ^ (cellY & 1) ? argbA : argbB;
            int pixelIdx = (x + (y * width)) * PIXEL_WIDTH;
            texture.pixels[pixelIdx + 3] = (argb >> 24) & 0xFF;
            texture.pixels[pixelIdx + 2] = (argb >> 16) & 0xFF;
            texture.pixels[pixelIdx + 1] = (argb >> 8) & 0xFF;
            texture.pixels[pixelIdx + 0] = (argb >> 0) & 0xFF;
            if (y % checkerSize == 0) { cellY += 1; }
        }
        if (x % checkerSize == 0) { cellX += 1; }
    }
    return texture;
}

Texture::Texture(int width, int height, int channels)
    : width(width)
    , height(height)
    , pitch(width * channels)
    , channels(channels)
    , pixels(width * height * channels)
{}

const Color Texture::sample(int x, int y) const
{
    // FIXME: Assumes RGBA
    const uint8_t* pixel = pixels.data() + x * channels + y * pitch;
    const uint8_t byte_r = pixel[2];
    const uint8_t byte_g = pixel[1];
    const uint8_t byte_b = pixel[0];
    const float r = byte_r / 255.0f;
    const float g = byte_g / 255.0f;
    const float b = byte_b / 255.0f;
    return Color(r, g, b);
}
