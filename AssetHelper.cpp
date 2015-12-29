#include "AssetHelper.hpp"
#include "SDL.h"
#include "Texture.hpp"

namespace assets
{
#include "bundled.inc"
}

Texture* AssetHelper::loadTexture(AssetHelper::ID id)
{
    auto surface = AssetHelper::loadSurface(id);
    auto texture = Texture::createFromSurface(surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Surface* AssetHelper::loadSurface(AssetHelper::ID id)
{
#define ASSET(__NAME__) { assets::ASSET_##__NAME__, sizeof(assets::ASSET_##__NAME__), }
    static struct { const void* buffer; int size; } ASSET_BUFFERS[] =
    {
        ASSET(FONT),
        ASSET(ICON),
    };
#undef ASSET

    auto asset = ASSET_BUFFERS[id];
    auto src = SDL_RWFromConstMem(asset.buffer, asset.size);
    return SDL_LoadBMP_RW(src, 1);
}
