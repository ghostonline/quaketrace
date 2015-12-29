#pragma once

namespace geometry
{
    struct Rect;
}

class Texture;
class FrameBuffer;

struct GraphicTool
{
    static void blit(FrameBuffer* dst, const geometry::Rect& dstRect, const Texture& src, const geometry::Rect& srcRect);
};
