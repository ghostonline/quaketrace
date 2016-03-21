#pragma once

#include <vector>
#include <cstdint>
#include <string>

struct BinaryWriter
{
    std::vector<std::uint8_t> stream;

    template<typename T>
    int write(const T& data) { return write(reinterpret_cast<const std::uint8_t*>(&data), sizeof(T)); }
    int write(const std::uint8_t* data, std::size_t size);
};

template<>
inline int BinaryWriter::write<std::string>(const std::string& data)
{
    return write(reinterpret_cast<const std::uint8_t*>(data.c_str()), data.size());
}
