#include "AppConfig.hpp"
#include "CommandLine.hpp"

namespace {

const int DEFAULT_SCREEN_WIDTH = 320;
const int DEFAULT_SCREEN_HEIGHT = 240;
const int DEFAULT_DETAIL_LEVEL = 1;
const int DEFAULT_SOFT_SHADOW_RAYS = 10;
const int DEFAULT_OCCLUSION_RAYS = 32;
const int DEFAULT_THREAD_COUNT = 4;

}

AppConfig::ParseResult AppConfig::parse(int argc, char const * const * const argv)
{
    ParseResult result{ParseResult::PARSE_FAILED, "failed"};
    
    util::CommandLine cmd;
    auto mapArg = cmd.add<std::string>("input", 'i');
    auto imageArg = cmd.add<std::string>("output", 'o');
    auto widthArg = cmd.add<int>("width", 'w', DEFAULT_SCREEN_WIDTH);
    auto heightArg = cmd.add<int>("height", 'h', DEFAULT_SCREEN_HEIGHT);
    auto detailArg = cmd.add<int>("detail", 'd', DEFAULT_DETAIL_LEVEL);
    auto occlusionArg = cmd.add<int>("occlusion", DEFAULT_OCCLUSION_RAYS);
    auto shadowsArg = cmd.add<int>("shadows", DEFAULT_SOFT_SHADOW_RAYS);
    auto threadsArg = cmd.add<int>("threads", 'j', DEFAULT_THREAD_COUNT);
    auto cameraArg = cmd.add<int>("camera", 'c', 0);
    auto cameraListArg = cmd.add<bool>("camera-list", 'l', false);
    auto showHelp = cmd.add<bool>("help", false);

    auto cmdResult = cmd.parse(argc, argv);
    if (!cmdResult.success)
    {
        result.error = cmdResult.error;
        return result;
    }

    if (showHelp.getValue())
    {
        result.result = ParseResult::SHOW_HELP;
        result.help = cmd.createHelpString(argv[0]);
        return result;
    }

    mapFile = mapArg.getValue();
    imageFile = imageArg.getValue();
    width = widthArg.getValue();
    height = heightArg.getValue();
    detail = detailArg.getValue();
    occlusionRayCount = occlusionArg.getValue();
    softshadowRayCount = shadowsArg.getValue();
    threads = threadsArg.getValue();
    cameraIdx = cameraArg.getValue();
    cameraList = cameraListArg.getValue();

    if (mapFile.empty())
    {
        result.error = "No map file specified";
        return result;
    }

    if (imageFile.empty())
    {
        result.error = "No image file specified";
        return result;
    }

    result.result = ParseResult::PARSE_SUCCESS;
    return result;
}
