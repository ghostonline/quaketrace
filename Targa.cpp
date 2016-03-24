#include "Targa.hpp"
#include "Image.hpp"
#include "BinaryWriter.hpp"
#include "Assert.hpp"
#include <string>

// Reference: http://paulbourke.net/dataformats/tga/
namespace {
    enum DataType
    {
        DATATYPE_EMPTY = 0,
        // uncompressed
        DATATYPE_PALETTE = 1,
        DATATYPE_RGB = 2,
        DATATYPE_BW = 3,
        // compressed
        DATATYPE_PALETTE_RLE = 9,
        DATATYPE_RGB_RLE = 10,
        DATATYPE_BW_RLE = 11,
        DATATYPE_PALETTE_HUFF_DELTA_RLE = 32,
        DATATYPE_PALETTE_HUFF_DELTA_RLE_4PASS = 33,
    };

    struct ImageDescriptor
    {
        enum Interleaving : std::uint8_t
        {
            NONE,
            TWO_WAY,
            FOUR_WAY,
            RESERVED,
        };

        std::uint8_t attributeCount:3;
        std::uint8_t reserved:2; // set to 0
        Interleaving interleaving:2;

        ImageDescriptor()
        : attributeCount(0)
        , reserved(0)
        , interleaving(NONE)
        {}
    };

    static_assert(sizeof(ImageDescriptor) == 1, "ImageDescriptor fits in a single byte");
}

const targa::Buffer targa::encode(const Image& image)
{
    static const int datatype = DATATYPE_RGB;
    static const std::string id = "raytrace";
    static const ImageDescriptor descriptor;

    ASSERT(id.size() < 0xFF);
    //ASSERT(image.format == FORMAT_ARGB); // Only ARGB is supported

    util::BinaryWriter writer;
    writer.write<std::uint8_t>(id.size()); // idlength
    writer.write<std::uint8_t>(0); // colourmaptype
    writer.write<std::uint8_t>(datatype); // datatypecode
    writer.write<std::uint16_t>(0); // colourmap index
    writer.write<std::uint16_t>(0); // colourmap entry count
    writer.write<std::uint8_t>(0); // bits per colourmap entry
    writer.write<std::uint16_t>(0); // x_origin (lower-left corner)
    writer.write<std::uint16_t>(0); // y_origin (lower-left corner)
    writer.write<std::uint16_t>(image.width); // width
    writer.write<std::uint16_t>(image.height); // height
    writer.write<std::uint8_t>(image.getPixelSize() << 3); // bitsperpixel
    writer.write(descriptor); // imagedescriptor
    writer.write(id);

    static const Image::Channel CHANNEL_ORDER[] = {
        Image::BLUE,
        Image::GREEN,
        Image::RED,
        Image::ALPHA,
    };
    static const int CHANNEL_COUNT = 4; // TODO: Deduce from actual array
    for (int row = image.height - 1; row >= 0; --row)
    {
        for (int col = 0; col < image.width; ++col)
        {
            int pixelIdx = (col + row * image.width) * image.getPixelSize();
            for (int channelIdx = 0; channelIdx < CHANNEL_COUNT; ++channelIdx)
            {
                const int offset = image.getChannelOffset(CHANNEL_ORDER[channelIdx]);
                writer.write<std::uint8_t>(image.pixels[pixelIdx + offset]);
            }
        }
    }
    return writer.stream;
}
