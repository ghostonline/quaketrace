#include "BinaryWriter.hpp"

int BinaryWriter::write(const std::uint8_t* data, std::size_t size)
{
    auto ptrIdx = stream.size();
    stream.resize(ptrIdx + size);
    std::memcpy(stream.data() + ptrIdx, data, size);
    return size;
}
