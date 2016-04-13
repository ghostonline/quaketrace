#pragma once

#include <string>

struct AppConfig
{
    struct ParseResult
    {
        enum Result
        {
            PARSE_SUCCESS,
            PARSE_FAILED,
            SHOW_HELP,
        };

        static const ParseResult CreateSuccess() { return ParseResult(PARSE_SUCCESS); }
        static const ParseResult CreateFailed(const std::string& error)
        {
            ParseResult result(PARSE_FAILED);
            result.error = error;
            return result;
        }
        static const ParseResult CreateHelp(const std::string& help)
        {
            ParseResult result(SHOW_HELP);
            result.help = help;
            return result;
        }

        Result result;
        std::string error;
        std::string help;

    private:
        ParseResult(Result result) : result(result) {}

    };

    ParseResult parse(int argc, char const * const * const argv);

    std::string mapFile;
    std::string imageFile;
    int cameraIdx;
    bool cameraList;
    
    int width;
    int height;

    int detail;
    int softshadowRayCount;
    int occlusionRayCount;

    int threads;
};
