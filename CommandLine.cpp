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

std::string util::CommandLine::createHelpString(const char* arg0) const
{
    std::vector<char> buffer;
    util::StringTool::append(&buffer, arg0);
    for (int ii = 0; ii < parsers.size(); ++ii)
    {
        util::StringTool::append(&buffer, " ");
        auto parameter = parsers[ii]->getParameterStr();
        util::StringTool::append(&buffer, parameter);
    }

    util::StringTool::append(&buffer, "\n\n");
    for (int ii = 0; ii < parsers.size(); ++ii)
    {
        parsers[ii]->appendHelp(&buffer);
        util::StringTool::append(&buffer, "\n\n");
    }

    std::string help(buffer.begin(), buffer.end());
    return help;
}

std::string util::Arg::getParameterStr() const
{
    std::vector<char> buffer;
    if (isRequired()) { buffer.push_back('('); }
    else { buffer.push_back('['); }
    appendFlag(&buffer, "|");
    if (isRequired()) { buffer.push_back(')'); }
    auto argumentType = getArgumentType();
    if (!argumentType.empty())
    {
        util::StringTool::append(&buffer, " <");
        util::StringTool::append(&buffer, argumentType);
        buffer.push_back('>');
    }
    if (!isRequired()) { buffer.push_back(']'); }
    return std::string(buffer.begin(), buffer.end());
}

void util::Arg::appendHelp(std::vector<char> *buffer) const
{
    appendFlag(buffer, ", ");
    if (!required && !getArgumentType().empty())
    {
        util::StringTool::append(buffer, " (defaults to ");
        appendResetValue(buffer);
        buffer->push_back(')');
    }
    util::StringTool::append(buffer, "\n\t");
    util::StringTool::appendWrapped(buffer, help, 60, "\n\t");
}

bool util::Arg::matchFlag(const char* arg) const
{
    if (!isFlag(arg)) { return false; }
    if (shorthand && arg[1] != COMMANDLINE_ARGUMENT_PREFIX)
    {
        return arg[1] == shorthand && arg[2] == '\0';
    }
    return arg[1] == COMMANDLINE_ARGUMENT_PREFIX && !flag.compare(&arg[2]);
}

bool util::Arg::isFlag(const char* arg) const { return arg[0] == COMMANDLINE_ARGUMENT_PREFIX; }

void util::Arg::appendFlag(std::vector<char>* buffer, const char* separator) const
{
    buffer->push_back(COMMANDLINE_ARGUMENT_PREFIX);
    buffer->push_back(COMMANDLINE_ARGUMENT_PREFIX);
    util::StringTool::append(buffer, flag);
    if (shorthand)
    {
        util::StringTool::append(buffer, separator);
        buffer->push_back(COMMANDLINE_ARGUMENT_PREFIX);
        buffer->push_back(shorthand);
    }
}

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
std::string util::ValueArg<bool>::getArgumentType() const
{
    return "";
}

template<>
void util::ValueArg<bool>::appendResetValue(std::vector<char>* buffer) const {}


template<>
const util::Arg::ParseResult util::ValueArg<std::string>::parse(int argc, char const * const * argv)
{
    if (!matchFlag(argv[0]))
    {
        return 0;
    }

    if (argc < 2 || isFlag(argv[1]))
    {
        std::string error = "Flag requires an argument: ";
        error += argv[0];
        return error.c_str();
    }

    value = argv[1];

    return 2;
}

template<>
std::string util::ValueArg<std::string>::getArgumentType() const
{
    return "string";
}

template<>
void util::ValueArg<std::string>::appendResetValue(std::vector<char>* buffer) const
{
    util::StringTool::append(buffer, resetValue);
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
        std::string error = "Flag requires an argument: ";
        error += argv[0];
        return error.c_str();
    }

    if (!util::StringTool::parseInteger(argv[1], &value))
    {
        std::string error = "No valid integer argument for flag: ";
        error += argv[0];
        return error.c_str();
    }

    return 2;
}

template<>
std::string util::ValueArg<int>::getArgumentType() const
{
    return "integer";
}

template<>
void util::ValueArg<int>::appendResetValue(std::vector<char>* buffer) const
{
    std::vector<char> resetValueStr(255);
    std::sprintf(resetValueStr.data(), "%d", resetValue);
    util::StringTool::append(buffer, resetValueStr.data());
}

template<>
const util::Arg::ParseResult util::ValueArg<float>::parse(int argc, char const * const * argv)
{
    if (!matchFlag(argv[0]))
    {
        return 0;
    }

    if (argc < 2)
    {
        std::string error = "Flag requires an argument: ";
        error += argv[0];
        return error.c_str();
    }

    if (!util::StringTool::parseFloat(argv[1], &value))
    {
        std::string error = "No valid number argument for flag: ";
        error += argv[0];
        return error.c_str();
    }

    return 2;
}

template<>
std::string util::ValueArg<float>::getArgumentType() const
{
    return "number";
}

template<>
void util::ValueArg<float>::appendResetValue(std::vector<char>* buffer) const
{
    std::vector<char> resetValueStr(255);
    std::sprintf(resetValueStr.data(), "%.1f", resetValue);
    util::StringTool::append(buffer, resetValueStr.data());
}
