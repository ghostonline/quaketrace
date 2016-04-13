#pragma once

#include "Math.hpp"
#include <vector>
#include <string>

namespace util
{

struct StringTool
{
    static const bool parseVec3f(const char* str, math::Vec3f* vec);
    static const bool parseFloat(const char* str, float* number);
    static const bool parseInteger(const char* str, int* integer);
    static void append(std::vector<char>* buffer, const char* text);
    static void append(std::vector<char>* buffer, const std::string& text);
};

}
