#pragma once
#include "Assert.hpp"

#define UTIL_ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

namespace util
{
    template<typename T>
    struct ArrayView
    {
        T const * const array;
        const int size;

        ArrayView(const T* array, int size) : array(array), size(size) {}
        ArrayView(const T (&array)[]) : array(array), size(UTIL_ARRAY_SIZE(array)) {}

        ArrayView slice(int startIdx, int endIdx) const
        {
            ASSERT(0 <= startIdx && startIdx < size);
            ASSERT(startIdx < endIdx && endIdx <= size);
            return { array + startIdx, endIdx - startIdx };
        }


        inline const T& operator[](int idx) const
        {
            ASSERT(0 <= idx && idx < size);
            return array[idx];
        }

        inline ArrayView<T> operator+(int idx) const
        {
            ASSERT(0 <= idx && idx < size);
            return { array + idx, size - idx };
        }

        inline ArrayView<T> operator-(int idx) const
        {
            return (*this) + (-idx);
        }

        inline operator const T*() const
        {
            return array;
        }
    };
}
