#pragma once

#include <string>
#include <vector>

namespace util
{

class Arg
{
public:
    struct ParseResult
    {
        ParseResult(int args) : args(args) {}
        ParseResult(const char* error) : args(0), error(error) {}

        int args;
        std::string error;
    };

    virtual const ParseResult parse(int argc, char const * const * argv) = 0;
    virtual void reset() = 0;

protected:
    Arg(const char* flag, char shorthand)
    : flag(flag)
    , shorthand(shorthand)
    {}

    bool matchFlag(const char* arg) const;
    bool isFlag(const char* arg) const;

private:
    std::string flag;
    char shorthand;
};

template<typename T>
class ValueArg : public Arg
{
public:
    ValueArg(const char* flag, char shorthand = 0, const T& resetValue = T())
    : Arg(flag, shorthand)
    , resetValue(resetValue)
    {}

    const T getValue() const { return value; }

    const ParseResult parse(int argc, char const * const * argv);
    void reset() { value = resetValue; }

private:
    T value;
    T resetValue;

};
    
class CommandLine
{
public:
    struct ParseResult
    {
        ParseResult(bool success) : success(success) {}
        ParseResult(const char* error) : success(false), error(error) {}
        ParseResult(const std::string& error) : success(false), error(error) {}

        bool success;
        std::string error;
    };

    CommandLine() {}

    template<typename T>
    ValueArg<T> add(const char* flag, char shorthand = 0, const T& resetValue = T());
    template<typename T>
    ValueArg<T> add(const char* flag, const T& resetValue) { return add(flag, 0, resetValue); }

    const ParseResult parse(int argc, char const * const * argv) const;

private:
    std::vector<Arg*> parsers;
};

template<typename T>
inline ValueArg<T> CommandLine::add(const char* flag, char shorthand, const T& resetValue)
{
    ValueArg<T> arg(flag, shorthand, resetValue);
    parsers.push_back(&arg);
    return arg;
}

}
