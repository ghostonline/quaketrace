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

void util::StringTool::append(std::vector<char>* buffer, const char* start, const char* end)
{
    auto size = end - start;
    auto bufferSize = buffer->size();
    buffer->reserve(bufferSize + size);
    buffer->insert(buffer->end(), start, end);
}

void util::StringTool::append(std::vector<char>* buffer, const char* text)
{
    auto size = strlen(text);
    append(buffer, text, text + size);
}

void util::StringTool::append(std::vector<char>* buffer, const std::string& text)
{
    const char* cStr = text.c_str();
    append(buffer, cStr, cStr + text.size());
}

void util::StringTool::appendWrapped(std::vector<char>* buffer, const std::string& text, int maxWidth, const char* separator)
{
    std::size_t length = 0;
    std::size_t wordLength = 0;
    const char* lastCut = text.c_str();
    for (const char* ptr = text.c_str(); *ptr != 0; ++ptr)
    {
        ++length;
        if (*ptr == ' ')
        {
            wordLength = length;
            continue;
        }

        if (length == maxWidth)
        {
            append(buffer, lastCut, lastCut + wordLength);
            append(buffer, separator);
            length = length - wordLength;
            lastCut += wordLength;
        }
    }

    if (length)
    {
        append(buffer, lastCut, lastCut + length);
    }
}
