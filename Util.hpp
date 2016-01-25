#pragma once

#define UTIL_ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

namespace util
{
    template<typename T>
    inline int lastIndex(const std::vector<T>& vec)
    {
        return static_cast<int>(vec.size() - 1);
    }

    template<typename T>
    inline const T* castFromMemory(const void* data, int offset = 0)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        return reinterpret_cast<const T*>(bytes + offset);
    }

}
