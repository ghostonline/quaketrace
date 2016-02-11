#include "AssetHelper.hpp"
#include "SDL.h"
#include "Texture.hpp"

namespace assets
{
#include "bundled.inc"
}

const void* AssetHelper::getRaw(AssetHelper::ID id, int* size)
{
#define ASSET(__NAME__) { assets::ASSET_##__NAME__, sizeof(assets::ASSET_##__NAME__), }
    static struct { const void* buffer; int size; } ASSET_BUFFERS[] =
    {
        ASSET(FONT),
        ASSET(ICON),
        ASSET(TESTMAP),
        ASSET(TESTMAP2),
    };
#undef ASSET

    auto asset = ASSET_BUFFERS[id];
    if (size) { *size = asset.size; }
    return asset.buffer;
}


Texture AssetHelper::loadTexture(AssetHelper::ID id)
{
    auto surface = AssetHelper::loadSurface(id);
    auto texture = Texture::createFromSDL(surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Surface* AssetHelper::loadSurface(AssetHelper::ID id)
{
    int size = 0;
    auto buffer = getRaw(id, &size);
    auto src = SDL_RWFromConstMem(buffer, size);
    return SDL_LoadBMP_RW(src, 1);
}
