#pragma once

#include "Texture.hpp"
struct SDL_Surface;

struct AssetHelper
{
    enum ID
    {
        FONT,
        ICON,
        TESTMAP,
        TESTMAP2,
        PALETTE,
    };

    static Texture loadTexture(ID id);
    static SDL_Surface* loadSurface(ID id);
    static const void* getRaw(ID id, int* size);
};
