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

    const ParseResult parse(int argc, char const * const * argv)
    {
        auto result = parseArgument(argc, argv);
        set = result.args > 0;
        return result;
    }
    void reset() { set = false; resetValue(); }
    std::string getParameterStr() const;
    void appendHelp(std::vector<char>* buffer) const;
    bool isSet() const { return set; }

protected:
    Arg(const char* flag, char shorthand, bool required, const char* help)
    : flag(flag)
    , help(help)
    , shorthand(shorthand)
    , required(required)
    , set(false)
    {}

    bool matchFlag(const char* arg) const;
    bool isFlag(const char* arg) const;
    void appendFlag(std::vector<char>* buffer, const char* separator) const;
    bool isRequired() const { return required; }

    virtual void resetValue() = 0;
    virtual const ParseResult parseArgument(int argc, char const * const * argv) = 0;
    virtual std::string getArgumentType() const = 0;
    virtual void appendDefaultValue(std::vector<char>* buffer) const = 0;

private:
    std::string flag;
    std::string help;
    char shorthand;
    bool required;
    bool set;
};

template<typename T>
class ValueArg : public Arg
{
public:
    typedef std::unique_ptr<ValueArg<T>> Ptr;

    ValueArg(const char* flag, char shorthand, const T& defaultValue, bool required, const char* help)
    : Arg(flag, shorthand, required, help)
    , defaultValue(defaultValue)
    {}

    const T getValue() const { return value; }

    std::string getParameterStr() const;

protected:
    virtual void resetValue() override { value = defaultValue; }
    virtual const ParseResult parseArgument(int argc, char const * const * argv) override;
    virtual std::string getArgumentType() const override;
    virtual void appendDefaultValue(std::vector<char>* buffer) const override;

private:
    T value;
    T defaultValue;
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
