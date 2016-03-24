#pragma once

#include <string>
#include <vector>

namespace util
{

class CommandLine
{
    static const char FLAG_PREFIX = '-';

public:
    CommandLine(int argc, char const * const * const argv)
    : argv(argv)
    , argc(argc)
    , isFlag(argc)
    {
        for (int ii = argc - 1; ii >= 0; --ii)
        {
            isFlag[ii] = argv[ii] && argv[ii][0] == FLAG_PREFIX;
        }
    }

    template<typename T>
    bool parse(T* value, const std::string& flag) const;
    template<typename T>
    bool parse(T* value, const std::string& flag, char abbreviation) const;

private:
    char const * const * const argv;
    int argc;
    std::vector<bool> isFlag;
};

template<typename T>
inline bool CommandLine::parse(T* value, const std::string& flag, char abbreviation) const
{
    if (!parse(value, flag))
    {
        std::string abbrString(&abbreviation, 1);
        return parse(value, abbrString.c_str());
    }
    return true;
}

}
