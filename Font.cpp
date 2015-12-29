#include "Font.hpp"
#include <SDL.h>
#include <cstring>
#include <cstdint>
#include <vector>
#include <memory>
#include "FrameBuffer.hpp"
#include "AssetHelper.hpp"

using namespace std;

const AssetHelper::ID FONT = AssetHelper::ID::FONT;
const int GLYPH_WIDTH = 7;
const int GLYPH_HEIGHT = 9;
const char GLYPH_CHARS[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const int COLOR_TRANSPARENT = 0xFF000000;

Font* Font::create()
{
    vector<Glyph> index;

    auto tex = AssetHelper::loadTexture(FONT);
    int glyphsPerRow = tex->getWidth() / GLYPH_WIDTH;

    // Build index
    for (int idx = 0; GLYPH_CHARS[idx] != 0; ++idx)
    {
        char code = GLYPH_CHARS[idx];
        while (index.size() < static_cast<size_t>(code + 1)) { index.push_back({ 0, 0, 0, 0 }); }

        int column = idx % glyphsPerRow;
        int row = idx / glyphsPerRow;
        index[code] = { column * GLYPH_WIDTH, row * GLYPH_HEIGHT, GLYPH_WIDTH, GLYPH_HEIGHT };
    }

    return new Font(tex, index);
}

void Font::blitString(FrameBuffer* fb, const char* text, int x, int y, Align align)
{
    typedef uint32_t pixel;

    auto length = strlen(text);
    int width = 0;
    int height = 0;
    vector<Glyph*> glyphs;
    for (size_t ii = 0; ii < length; ++ii)
    {
        char c = text[ii];
        if (index.size() <= static_cast<size_t>(c)) { continue; }

        auto& glyph = index[c];
        glyphs.push_back(&glyph);
        width += glyph.w;
        if (height < glyph.h) { height = glyph.h; }
    }

    vector<pixel> label(width * height);
    int writeX = 0;
    const pixel* pixels = reinterpret_cast<pixel*>(texture->getPixels());
    int texPitch = texture->getPitch() / sizeof(pixel);
    for (auto glyph : glyphs)
    {
        int glyphWidth = glyph->w * sizeof(pixel);
        for (int row = 0; row < glyph->h; ++row)
        {
            int startLabel = writeX + row * width;
            int startTex = glyph->x + (glyph->y + row) * texPitch;
            memcpy(label.data() + startLabel, pixels + startTex, glyphWidth);
        }
        writeX += glyph->w;
    }

    if (align == ALIGN_CENTER)
    {
        x -= width >> 1;
    }

    pixel* fbPixels = reinterpret_cast<pixel*>(fb->get());
    int fbPitch = fb->getPitch() / sizeof(pixel);
    for (int row = 0; row < height; ++row)
    {
        int fbStart = x + (y + row) * fbPitch;
        int labelStart = row * width;
        for (int idx = 0; idx < width; ++idx)
        {
            pixel p = label[labelStart + idx];
            if (p == COLOR_TRANSPARENT) { continue; }
            fbPixels[fbStart + idx] = p;
        }
    }
}
