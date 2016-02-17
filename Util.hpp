#pragma once

#include "ArrayView.hpp"

namespace util
{
    template<typename T>
    inline int lastIndex(const std::vector<T>& vec)
    {
        return static_cast<int>(vec.size() - 1);
    }

    template<typename T>
    inline int lastIndex(const ArrayView<T>& view)
    {
        return static_cast<int>(view.size - 1);
    }

    template<typename T>
    inline const T* castFromMemory(const void* data, int offset = 0)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        return reinterpret_cast<const T*>(bytes + offset);
    }

    template<typename T>
    inline int findItemInArray(const ArrayView<T>& array, const T& item)
    {
        for (int ii = lastIndex(array); ii >= 0; --ii)
        {
            if (array[ii] == item)
            {
                return ii;
            }
        }
        return -1;
    }
}
