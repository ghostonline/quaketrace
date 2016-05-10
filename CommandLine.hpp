#pragma once

#include <string>
#include <vector>
#include <memory>

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
    virtual std::string getParameterStr() const = 0;

protected:
    Arg(const char* flag, char shorthand, bool required)
    : flag(flag)
    , shorthand(shorthand)
    , required(required)
    {}

    bool matchFlag(const char* arg) const;
    bool isFlag(const char* arg) const;
    void appendFlag(std::vector<char>* buffer) const;
    bool isRequired() const { return required; }

private:
    std::string flag;
    char shorthand;
    bool required;
};

template<typename T>
class ValueArg : public Arg
{
public:
    typedef std::unique_ptr<ValueArg<T>> Ptr;

    ValueArg(const char* flag, char shorthand, const T& resetValue, bool required)
    : Arg(flag, shorthand, required)
    , resetValue(resetValue)
    {}

    const T getValue() const { return value; }

    const ParseResult parse(int argc, char const * const * argv);
    void reset() { value = resetValue; }
    std::string getParameterStr() const;

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
    typename ValueArg<T>::Ptr add(const char* flag) { return add(flag, 0, T(), true); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, const T& resetValue) { return add(flag, 0, resetValue, false); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand) { return add(flag, shorthand, T(), true); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand, const T& resetValue)  { return add(flag, shorthand, resetValue, false); }

    const ParseResult parse(int argc, char const * const * argv) const;

    std::string createHelpString(const char* arg0) const;

private:
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand, const T& resetValue, bool required);

    std::vector<Arg*> parsers;
};

template<typename T>
inline typename ValueArg<T>::Ptr CommandLine::add(const char* flag, char shorthand, const T& resetValue, bool required)
{
    // TODO: Fix freed memory access error if arg is freed before CommandLine::parse is executed
    typename ValueArg<T>::Ptr arg(new ValueArg<T>(flag, shorthand, resetValue, required));
    parsers.push_back(arg.get());
    return arg;
}

}
