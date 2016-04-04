#include "AppConfig.hpp"
#include "CommandLine.hpp"

namespace {

const int DEFAULT_SCREEN_WIDTH = 640;
const int DEFAULT_SCREEN_HEIGHT = 480;
const int DEFAULT_DETAIL_LEVEL = 1;
const int DEFAULT_SOFT_SHADOW_RAYS = 10;
const int DEFAULT_OCCLUSION_RAYS = 32;
const int DEFAULT_THREAD_COUNT = 4;

}

AppConfig::ParseResult AppConfig::parse(int argc, char const * const * const argv)
{
    ParseResult result{false, "Parsing failed"};
    
    util::CommandLine cmd(argc, argv);
    if (!cmd.parse(&mapFile, "input", 'i'))
    {
        result.error = "No input map file found";
        return result;
    }
    if (!cmd.parse(&imageFile, "output", 'o'))
    {
        result.error = "No output tga file found";
        return result;
    }

    if (!cmd.parse(&width, "width", 'w')) { width = DEFAULT_SCREEN_WIDTH; }
    if (!cmd.parse(&height, "height", 'h')) { height = DEFAULT_SCREEN_HEIGHT; }
    if (!cmd.parse(&detail, "detail", 'd')) { detail = DEFAULT_DETAIL_LEVEL; }
    if (!cmd.parse(&occlusionRayCount, "occlusion")) { occlusionRayCount = DEFAULT_OCCLUSION_RAYS; }
    if (!cmd.parse(&softshadowRayCount, "shadows")) { softshadowRayCount = DEFAULT_SOFT_SHADOW_RAYS; }
    if (!cmd.parse(&threads, "threads", 'j')) { threads = DEFAULT_THREAD_COUNT; }

    result.success = true;
    return result;
}
