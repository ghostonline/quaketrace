#pragma once

class Texture;
struct SDL_Surface;

struct AssetHelper
{
    enum ID
    {
        FONT,
        ICON,
    };

    static Texture* loadTexture(ID id);
    static SDL_Surface* loadSurface(ID id);
};
