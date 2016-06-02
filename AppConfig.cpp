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
    auto mapArg = cmd.add<std::string>("input", 'i', "Path to a compiled Quake 1 level file");
    auto imageArg = cmd.add<std::string>("output", 'o', "Path to the TGA file that will be written");
    auto widthArg = cmd.add<int>("width", 'w', DEFAULT_SCREEN_WIDTH, "Width of the generated image");
    auto heightArg = cmd.add<int>("height", 'h', DEFAULT_SCREEN_HEIGHT, "Height of the generated image");
    auto detailArg = cmd.add<int>("detail", 'd', DEFAULT_DETAIL_LEVEL, "Supersampling factor to apply. A value of 2 results in 4 samples per pixel, 3 results in 9 samples, 4 in 16 samples etc.");
    auto occlusionArg = cmd.add<int>("occlusion", DEFAULT_OCCLUSION_RAYS, "Number of rays to cast for ambient occlusion detection");
    auto occlusionStrengthArg = cmd.add<int>("occlusion-strength", DEFAULT_OCCLUSION_STRENGTH, "Occlusion ray length, a higher value will grow ambient occlusion shadows");
    auto shadowsArg = cmd.add<int>("shadows", DEFAULT_SOFT_SHADOW_RAYS, "Number of soft shadow rays");
    auto threadsArg = cmd.add<int>("threads", 'j', DEFAULT_THREAD_COUNT, "Number of worker threads to use while raytracing, best set to the number of CPU cores");
    auto cameraArg = cmd.add<int>("camera", 'c', 0, "Intermission camera index to use as viewpoint");
    auto cameraListArg = cmd.add<bool>("camera-list", 'l', false, "Print the number of intermission cameras in the level file");
    auto gammaArg = cmd.add<float>("gamma", 1.0f, "Apply gamma correction to the generated image");
    auto showHelp = cmd.add<bool>("help", false, "Display program usage information");

    auto cmdResult = cmd.parse(argc, argv);
    if (!cmdResult.success)
    {
        return ParseResult::CreateFailed(cmdResult.error);
    }

    if (showHelp->getValue())
    {
        auto help = cmd.createHelpString(argv[0]);
        return ParseResult::CreateHelp(help);
    }

    mapFile = mapArg->getValue();
    imageFile = imageArg->getValue();
    width = widthArg->getValue();
    height = heightArg->getValue();
    detail = detailArg->getValue();
    occlusionRayCount = occlusionArg->getValue();
    occlusionStrength = occlusionStrengthArg->getValue();
    softshadowRayCount = shadowsArg->getValue();
    threads = threadsArg->getValue();
    cameraIdx = cameraArg->getValue();
    cameraList = cameraListArg->getValue();
    gamma = gammaArg->getValue();

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
