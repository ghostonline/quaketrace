#include "StringTool.hpp"
#include <string>

const bool util::StringTool::parseVec3f(const char* str, math::Vec3f* vec)
{
    float axis[3];
    parseFloat(str, &axis[0]);
    for (int components = 2; components > 0;)
    {
        switch (*str)
        {
        case ' ':
            if (!parseFloat(str, &axis[3 - components]))
            {
                return false;
            }
            --components;
            break;
        case 0:
            components = 0;
            break;
        }
        ++str;
    }

    *vec = {axis[0], axis[1], axis[2]};
    return true;
}

const bool util::StringTool::parseFloat(const char* str, float* number)
{
    try
    {
        *number = std::stof(str);
    }
    catch (std::invalid_argument a)
    {
        return false;
    }
    return true;
}

const bool util::StringTool::parseInteger(const char* str, int* integer)
{
    try
    {
        *integer = std::stoi(str);
    }
    catch (std::invalid_argument a)
    {
        return false;
    }
    return true;
}
