#pragma once

#include <string>

struct AppConfig
{
    struct ParseResult
    {
        bool success;
        std::string error;
    };

    ParseResult parse(int argc, char const * const * const argv);

    std::string mapFile;
    std::string imageFile;
    int cameraIdx;
    
    int width;
    int height;

    int detail;
    int softshadowRayCount;
    int occlusionRayCount;

    int threads;
};
