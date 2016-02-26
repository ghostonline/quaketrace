#include "StringTool.hpp"
#include <string>

const math::Vec3f util::StringTool::parseVec3f(const char* str)
{
    float axis[3];
    axis[0] = parseFloat(str);
    for (int components = 2; components > 0;)
    {
        switch (*str)
        {
        case ' ':
            axis[3 - components] = parseFloat(str);
            --components;
            break;
        case 0:
            components = 0;
            break;
        }
        ++str;
    }
    return {axis[0], axis[1], axis[2]};
}

const float util::StringTool::parseFloat(const char* str)
{
    return std::stof(str);
}

const int util::StringTool::parseInteger(const char* str)
{
    return std::stoi(str);
}
