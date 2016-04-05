#include "Console.hpp"
#include "AppConfig.hpp"
#include "File.hpp"
#include "Scene.hpp"
#include "BspLoader.hpp"
#include "BackgroundTracer.hpp"
#include "Targa.hpp"
#include <cstdio>
#include <cstdlib>

int Console::runUntilFinished(int argc, char const * const * const argv)
{
    AppConfig config;
    {
        auto parseResult = config.parse(argc, argv);
        if (!parseResult.success)
        {
            std::printf("Parse failed: %s", parseResult.error.c_str());
            return EXIT_FAILURE;
        }
    }

    File mapFile = File::open(config.mapFile.c_str());
    if (!mapFile.isValid())
    {
        std::printf("Could not open map file: %s", config.mapFile.c_str());
        return EXIT_FAILURE;
    }
    size_t mapDataSize = mapFile.size();
    std::vector<std::uint8_t> mapData(mapDataSize);
    mapFile.read(mapData.data(), mapDataSize);
    Scene scene = BspLoader::createSceneFromBsp(mapData.data(), mapDataSize);

    // Correct for aspect ratio
    scene.camera.halfViewAngles.y *= config.height / static_cast<float>(config.width);

    RayTracer::Config traceConfig;
    traceConfig.detail = config.detail;
    traceConfig.occlusionRayCount = config.occlusionRayCount;
    traceConfig.softshadowRayCount = config.softshadowRayCount;
    traceConfig.threads = config.threads;
    traceConfig.width = config.width;
    traceConfig.height = config.height;
    BackgroundTracer engine(traceConfig);

    engine.startTrace(scene);

    int percentage = 0;
    do
    {
        int newPercentage = static_cast<int>(engine.getProgress() * 100.0f);
        if (percentage != newPercentage)
        {
            percentage = newPercentage;
            if ((percentage % 10) == 0)
            {
                std::printf("%d%%", percentage);
            }
            else
            {
                std::printf(".");
            }
        }
    } while (engine.isTracing());

    std::printf("\n");

    auto tga = targa::encode(engine.getCanvas());
    File f = File::openW(config.imageFile.c_str());
    if (!f.isValid())
    {
        std::printf("Could not write to file: %s", config.imageFile.c_str());
        return EXIT_FAILURE;
    }
    f.write(tga.data(), tga.size());

    return EXIT_SUCCESS;
}
