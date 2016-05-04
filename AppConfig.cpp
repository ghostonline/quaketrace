#include "AppConfig.hpp"
#include "CommandLine.hpp"

namespace {

const int DEFAULT_SCREEN_WIDTH = 320;
const int DEFAULT_SCREEN_HEIGHT = 240;
const int DEFAULT_DETAIL_LEVEL = 1;
const int DEFAULT_SOFT_SHADOW_RAYS = 10;
const int DEFAULT_OCCLUSION_RAYS = 32;
const int DEFAULT_OCCLUSION_STRENGTH = 16;
const int DEFAULT_THREAD_COUNT = 4;

}

AppConfig::ParseResult AppConfig::parse(int argc, char const * const * const argv)
{
    util::CommandLine cmd;
    auto mapArg = cmd.add<std::string>("input", 'i');
    auto imageArg = cmd.add<std::string>("output", 'o');
    auto widthArg = cmd.add<int>("width", 'w', DEFAULT_SCREEN_WIDTH);
    auto heightArg = cmd.add<int>("height", 'h', DEFAULT_SCREEN_HEIGHT);
    auto detailArg = cmd.add<int>("detail", 'd', DEFAULT_DETAIL_LEVEL);
    auto occlusionArg = cmd.add<int>("occlusion", DEFAULT_OCCLUSION_RAYS);
    auto occlusionStrengthArg = cmd.add<int>("occlusion-strength", DEFAULT_OCCLUSION_STRENGTH);
    auto shadowsArg = cmd.add<int>("shadows", DEFAULT_SOFT_SHADOW_RAYS);
    auto threadsArg = cmd.add<int>("threads", 'j', DEFAULT_THREAD_COUNT);
    auto cameraArg = cmd.add<int>("camera", 'c', 0);
    auto cameraListArg = cmd.add<bool>("camera-list", 'l', false);
    auto showHelp = cmd.add<bool>("help", false);

    auto cmdResult = cmd.parse(argc, argv);
    if (!cmdResult.success)
    {
        return ParseResult::CreateFailed(cmdResult.error);
    }

    if (showHelp.getValue())
    {
        auto help = cmd.createHelpString(argv[0]);
        return ParseResult::CreateHelp(help);
    }

    mapFile = mapArg.getValue();
    imageFile = imageArg.getValue();
    width = widthArg.getValue();
    height = heightArg.getValue();
    detail = detailArg.getValue();
    occlusionRayCount = occlusionArg.getValue();
    occlusionStrength = occlusionStrengthArg.getValue();
    softshadowRayCount = shadowsArg.getValue();
    threads = threadsArg.getValue();
    cameraIdx = cameraArg.getValue();
    cameraList = cameraListArg.getValue();

    if (mapFile.empty())
    {
        return ParseResult::CreateFailed("No map file specified");
    }

    if (imageFile.empty() && !cameraList)
    {
        return ParseResult::CreateFailed("No image file specified");
    }

    return ParseResult::CreateSuccess();
}
