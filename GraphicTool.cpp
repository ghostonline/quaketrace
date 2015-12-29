#include "GraphicTool.hpp"
#include "Geometry.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include <cstdint>
#include <memory>

using namespace std;
using namespace geometry;

inline int min(int x, int y) { return x < y ? x : y; }

void GraphicTool::blit(FrameBuffer* dst, const Rect& dstRect, const Texture& src, const Rect& srcRect)
{
    uint8_t* dstPixels = reinterpret_cast<uint8_t*>(dst->get());
    uint8_t* srcPixels = reinterpret_cast<uint8_t*>(src.getPixels());
    int width = min(dstRect.width, srcRect.width) * sizeof(uint32_t);
    int height = min(dstRect.height, srcRect.height);
    for (int row = 0; row < height; ++row)
    {
        int srcY = srcRect.y + row;
        int srcX = (srcRect.x) * sizeof(uint32_t);
        int dstY = dstRect.y + row;
        int dstX = (dstRect.x) * sizeof(uint32_t);
        uint8_t* srcStart = srcPixels + srcX + srcY * src.getPitch();
        uint8_t* dstStart = dstPixels + dstX + dstY * dst->getPitch();
        memcpy(dstStart, srcStart, width);
    }
}
