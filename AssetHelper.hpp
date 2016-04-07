#pragma once

struct AssetHelper
{
    enum ID
    {
        FONT,
        ICON,
        TESTMAP,
        TESTMAP2,
        PALETTE,
        E1M1,
    };

    static const void* getRaw(ID id, int* size);
};
