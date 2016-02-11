#include "Texture.hpp"
#include "SDL.h"
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

Texture Texture::createFromSDL(SDL_Surface *surface)
{
    auto rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

    const int pixelWidth = rgbaSurface->format->BitsPerPixel / 8;
    Texture texture(rgbaSurface->w, rgbaSurface->h, pixelWidth);

    auto sourcePixels = reinterpret_cast<const std::uint8_t*>(rgbaSurface->pixels);
    for (int ii = 0; ii < texture.width * texture.height; ++ii)
    {
        int pixelIdx = ii * pixelWidth;
        for (int channel = 0; channel < pixelWidth; ++channel)
        {
            const int pixelChannelIdx = pixelIdx + channel;
            texture.pixels[pixelChannelIdx] = sourcePixels[pixelChannelIdx];
        }
    }

    SDL_FreeSurface(rgbaSurface);

    return texture;
}


Texture::Texture(int width, int height, int channels)
    : width(width)
    , height(height)
    , pitch(width * channels)
    , pixels(width * height * channels)
{}
