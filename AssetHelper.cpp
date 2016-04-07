#include "AssetHelper.hpp"

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
        ASSET(PALETTE),
        ASSET(E1M1),
    };
#undef ASSET

    auto asset = ASSET_BUFFERS[id];
    if (size) { *size = asset.size; }
    return asset.buffer;
}
