#pragma once

#include <vector>
#include <cstdint>

struct SDL_Surface;

class Texture
{
public:
    static Texture createFromSDL(SDL_Surface* surface);
    static Texture createFromIndexedRGB(int width, int height, const void* indices, const void* palette);

    Texture(int width, int height, int channels);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getPitch() const { return pitch; }
    std::uint8_t* getPixels() { return pixels.data(); }
    const std::uint8_t* getPixels() const { return pixels.data(); }


private:
    int width;
    int height;
    int pitch;
    std::vector<std::uint8_t> pixels;
};
