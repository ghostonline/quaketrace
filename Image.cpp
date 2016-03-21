#include "Image.hpp"
#include "Assert.hpp"
#include "Util.hpp"

const int Image::FORMAT_PIXEL_SIZE[] = {
    4,
};

#define CHANNEL(r, g, b, a) {{r, g, b, a}}
const Image::Offset Image::CHANNEL_OFFSET_LOOKUP[] = {
    CHANNEL(2, 1, 0, 3),
};

Image::Image(int width, int height, Format format)
: width(width)
, height(height)
, pixels(width * height * FORMAT_PIXEL_SIZE[format])
{}

Image::Image(const std::vector<std::uint8_t>& pixels, int width, int height, Format format)
: width(width)
, height(height)
, pixels(pixels)
, format(format)
{
    ASSERT(width * height * FORMAT_PIXEL_SIZE[format] == pixels.size());
}
