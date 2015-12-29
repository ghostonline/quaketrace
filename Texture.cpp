#include "Texture.hpp"
#include "SDL.h"

Texture* Texture::create(const char* file)
{
    auto surface = SDL_LoadBMP(file);
    SDL_assert(surface != nullptr);
    auto texture = Texture::createFromSurface(surface);
    SDL_FreeSurface(surface);
    return texture;
}

Texture* Texture::createFromSurface(SDL_Surface* surface)
{
    auto rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_assert(rgbaSurface != nullptr);
    return new Texture(rgbaSurface);
}


Texture::Texture(SDL_Surface* surface)
    : surface(surface)
    , width(surface->w)
    , height(surface->h)
    , pitch(surface->pitch)
    , pixels(surface->pixels)
{}

Texture::~Texture()
{
    SDL_FreeSurface(surface);
}
