#pragma once

#define UTIL_ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

namespace util
{
    template<typename T>
    inline int lastIndex(const std::vector<T>& vec)
    {
        return static_cast<int>(vec.size() - 1);
    }
}
