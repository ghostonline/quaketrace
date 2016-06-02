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
    std::string getParameterStr() const;
    void appendHelp(std::vector<char>* buffer) const;

protected:
    Arg(const char* flag, char shorthand, bool required, const char* help)
    : flag(flag)
    , help(help)
    , shorthand(shorthand)
    , required(required)
    {}

    bool matchFlag(const char* arg) const;
    bool isFlag(const char* arg) const;
    void appendFlag(std::vector<char>* buffer, const char* separator) const;
    bool isRequired() const { return required; }

    virtual std::string getArgumentType() const = 0;
    virtual void appendResetValue(std::vector<char>* buffer) const = 0;

private:
    std::string flag;
    std::string help;
    char shorthand;
    bool required;
};

template<typename T>
class ValueArg : public Arg
{
public:
    typedef std::unique_ptr<ValueArg<T>> Ptr;

    ValueArg(const char* flag, char shorthand, const T& resetValue, bool required, const char* help)
    : Arg(flag, shorthand, required, help)
    , resetValue(resetValue)
    {}

    const T getValue() const { return value; }

    const ParseResult parse(int argc, char const * const * argv);
    void reset() { value = resetValue; }
    std::string getParameterStr() const;

protected:
    virtual std::string getArgumentType() const;
    virtual void appendResetValue(std::vector<char>* buffer) const;

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
    typename ValueArg<T>::Ptr add(const char* flag, const char* help) { return add(flag, 0, T(), true, help); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, const T& resetValue, const char* help) { return add(flag, 0, resetValue, false, help); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand, const char* help) { return add(flag, shorthand, T(), true, help); }
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand, const T& resetValue, const char* help)  { return add(flag, shorthand, resetValue, false, help); }

    const ParseResult parse(int argc, char const * const * argv) const;

    std::string createHelpString(const char* arg0) const;

private:
    template<typename T>
    typename ValueArg<T>::Ptr add(const char* flag, char shorthand, const T& resetValue, bool required, const char* help);

    std::vector<Arg*> parsers;
};

template<typename T>
inline typename ValueArg<T>::Ptr CommandLine::add(const char* flag, char shorthand, const T& resetValue, bool required, const char* help)
{
    // TODO: Fix freed memory access error if arg is freed before CommandLine::parse is executed
    typename ValueArg<T>::Ptr arg(new ValueArg<T>(flag, shorthand, resetValue, required, help));
    parsers.push_back(arg.get());
    return arg;
}

}
