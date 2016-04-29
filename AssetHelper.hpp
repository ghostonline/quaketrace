#pragma once

struct AssetHelper
{
    enum ID
    {
        FONT,
        ICON,
        PALETTE,
    };

    static const void* getRaw(ID id, int* size);
};
