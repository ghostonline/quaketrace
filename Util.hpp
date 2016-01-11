#pragma once

namespace util
{
    template<typename T>
    inline int lastIndex(const std::vector<T>& vec)
    {
        return static_cast<int>(vec.size() - 1);
    }
}
