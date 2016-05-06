#pragma once

#include <vector>
#include <cstdint>

struct Image;

namespace targa
{
    typedef std::vector<std::uint8_t> Buffer;
    const Buffer encode(const Image& image, const char* identifier = "");
};
