#include "CommandLine.hpp"
#include "StringTool.hpp"

template<>
bool util::CommandLine::parse(std::string* value, const std::string& flag) const
{
    for (int ii = 0; ii < argc - 1; ++ii)
    {
        if (isFlag[ii] && !isFlag[ii + 1] && !flag.compare(argv[ii] + 1))
        {
            *value = argv[ii + 1];
            return true;
        }
    }

    return false;
}

template<>
bool util::CommandLine::parse(int* value, const std::string& flag) const
{
    for (int ii = 0; ii < argc - 1; ++ii)
    {
        if (isFlag[ii] && !isFlag[ii + 1] && !flag.compare(argv[ii] + 1))
        {
            *value = util::StringTool::parseInteger(argv[ii + 1]);
            return true;
        }
    }

    return false;
}
