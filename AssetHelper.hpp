#pragma once

class Texture;
struct SDL_Surface;

struct AssetHelper
{
    enum ID
    {
        FONT,
        ICON,
        TESTMAP,
        TESTMAP2,
    };

    static Texture* loadTexture(ID id);
    static SDL_Surface* loadSurface(ID id);
    static const void* getRaw(ID id, int* size);
};
