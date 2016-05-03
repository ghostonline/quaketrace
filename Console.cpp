#include "Console.hpp"
#include "AppConfig.hpp"
#include "File.hpp"
#include "Scene.hpp"
#include "BspLoader.hpp"
#include "BackgroundTracer.hpp"
#include "Targa.hpp"
#include "Util.hpp"
#include "Math.hpp"
#include "Common.hpp"
#include <cstdio>
#include <cstdlib>

int Console::runUntilFinished(int argc, char const * const * const argv)
{
    AppConfig config;
    {
        auto parseResult = config.parse(argc, argv);
        if (parseResult.result == AppConfig::ParseResult::PARSE_FAILED)
        {
            std::printf("Parse failed: %s\n", parseResult.error.c_str());
            return EXIT_FAILURE;
        }

        if (parseResult.result == AppConfig::ParseResult::SHOW_HELP)
        {
            std::printf("%s\n", parseResult.help.c_str());
            return EXIT_SUCCESS;
        }
    }

    Scene scene;
    if (!common::loadBSP(config.mapFile.c_str(), &scene, config.width, config.height))
    {
        std::printf("Could not open map file: %s\n", config.mapFile.c_str());
        return EXIT_FAILURE;
    }

    if (config.cameraList)
    {
        std::printf("%lu\n", scene.cameras.size());
        return EXIT_SUCCESS;
    }

    RayTracer::Config traceConfig = common::parseRayTracerConfig(config);
    BackgroundTracer engine(traceConfig);

    size_t cameraIdx = math::clamp<size_t>(config.cameraIdx, 0, scene.cameras.size() - 1);
    engine.startTrace(scene, scene.cameras[cameraIdx]);

    std::printf("Starting tracing scene\n");
    int percentage = 0;
    do
    {
        int newPercentage = static_cast<int>(engine.getProgress() * 100.0f);
        if (percentage != newPercentage)
        {
            percentage = newPercentage;
            if ((percentage % 10) == 0)
            {
                std::printf("%3d%%\n", percentage);
            }
            else
            {
                std::printf(".");
                std::fflush(stdout);
            }
        }
        std::this_thread::yield();
    } while (engine.isTracing());

    std::printf("Trace complete\n");

    if (!common::writeToTGA(engine.getCanvas(), config.imageFile.c_str()))
    {
        std::printf("Could not write to file: %s\n", config.imageFile.c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
