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

        Result result;
        std::string error;
        std::string help;
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
