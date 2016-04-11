#pragma once

#include "Math.hpp"

namespace util
{

struct StringTool
{
    static const bool parseVec3f(const char* str, math::Vec3f* vec);
    static const bool parseFloat(const char* str, float* number);
    static const bool parseInteger(const char* str, int* integer);
};

}
