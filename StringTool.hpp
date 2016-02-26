#pragma once

#include "Math.hpp"

namespace util
{

struct StringTool
{
    static const math::Vec3f parseVec3f(const char* str);
    static const float parseFloat(const char* str);
    static const int parseInteger(const char* str);
};

}
