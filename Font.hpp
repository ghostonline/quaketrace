#pragma once

#include <memory>
#include <vector>
#include "Texture.hpp"

class FrameBuffer;

class Font
{
public:
    enum Align
    {
        ALIGN_LEFT,
        ALIGN_CENTER,
    };

    static Font* create();
    void blitString(FrameBuffer* fb, const char* text, int x, int y, Align align = ALIGN_LEFT);

private:
    struct Glyph { int x, y, w, h; };

    Font(const Texture& texture, const std::vector<Glyph>& index) : texture(texture), index(index) {}

    Texture texture;
    std::vector<Glyph> index;
};
