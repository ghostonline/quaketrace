#include "Targa.hpp"
#include "Image.hpp"
#include "Assert.hpp"

namespace {
    struct Header {
        std::uint8_t  idlength;
        std::uint8_t  colourmaptype;
        std::uint8_t  datatypecode;
        std::uint16_t colourmaporigin;
        std::uint16_t colourmaplength;
        std::uint8_t  colourmapdepth;
        std::uint16_t x_origin;
        std::uint16_t y_origin;
        std::uint16_t width;
        std::uint16_t height;
        std::uint8_t  bitsperpixel;
        std::uint8_t  imagedescriptor;
    };
}

const targa::Buffer targa::encode(const Image& image)
{
    Header hdr;
    return Buffer();
}
