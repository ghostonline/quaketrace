#include "CommandLine.hpp"
#include "StringTool.hpp"
#include "Util.hpp"

static const char COMMANDLINE_ARGUMENT_PREFIX = '-';

const util::CommandLine::ParseResult util::CommandLine::parse(int argc, char const * const * argv) const
{
    for (int ii = util::lastIndex(parsers); ii >= 0; --ii)
    {
        parsers[ii]->reset();
    }

    for (int argIdx = 1; argIdx < argc;)
    {
        int currentIdx = argIdx;
        for (int ii = util::lastIndex(parsers); ii >= 0; --ii)
        {
            auto result = parsers[ii]->parse(argc - argIdx, &argv[argIdx]);
            if (result.args)
            {
                argIdx += result.args;
                break;
            }

            if (result.error.size())
            {
                return result.error;
            }
        }

        if (currentIdx == argIdx)
        {
            // Unparsed argument
            std::string error = "Unparsed argument: ";
            error += argv[argIdx];
            return error;
        }
    }

    return true;
}

bool util::Arg::matchFlag(const char* arg) const
{
    if (!isFlag(arg)) { return false; }
    if (shorthand && arg[1] != COMMANDLINE_ARGUMENT_PREFIX)
    {
        return arg[1] == shorthand && arg[2] == '\0';
    }
    return arg[2] == COMMANDLINE_ARGUMENT_PREFIX && !flag.compare(&arg[3]);
}

bool util::Arg::isFlag(const char* arg) const { return arg[0] == COMMANDLINE_ARGUMENT_PREFIX; }

template<>
const util::Arg::ParseResult util::ValueArg<bool>::parse(int argc, char const * const * argv)
{
    if (!matchFlag(argv[0]))
    {
        return 0;
    }

    value = true;

    return 1;
}

template<>
const util::Arg::ParseResult util::ValueArg<std::string>::parse(int argc, char const * const * argv)
{
    if (!matchFlag(argv[0]))
    {
        return 0;
    }

    if (argc < 2 || isFlag(argv[1]))
    {
        return "Requires argument";
    }

    value = argv[1];

    return 2;
}

template<>
const util::Arg::ParseResult util::ValueArg<int>::parse(int argc, char const * const * argv)
{
    if (!matchFlag(argv[0]))
    {
        return 0;
    }

    if (argc < 2)
    {
        return "Requires argument";
    }

    if (!util::StringTool::parseInteger(argv[1], &value))
    {
        std::string error = "No valid integer argument for flag ";
        error += argv[0];
        return error.c_str();
    }

    return 2;
}
