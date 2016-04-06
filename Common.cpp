#include "Common.hpp"
#include "BspLoader.hpp"
#include "Scene.hpp"
#include "AppConfig.hpp"
#include "RayTracer.hpp"
#include "File.hpp"
#include "Targa.hpp"
#include "Util.hpp"

bool common::loadBSP(const char* filename, Scene* scene, int screenWidth, int screenHeight)
{
    File mapFile = File::open(filename);
    if (!mapFile.isValid())
    {
        return false;
    }
    size_t mapDataSize = mapFile.size();
    std::vector<std::uint8_t> mapData(mapDataSize);
    mapFile.read(mapData.data(), mapDataSize);
    *scene = BspLoader::createSceneFromBsp(mapData.data(), mapDataSize);

    // Correct for aspect ratio
    for (int ii = util::lastIndex(scene->cameras); ii >= 0; --ii)
    {
        scene->cameras[ii].halfViewAngles.y *= screenHeight / static_cast<float>(screenWidth);
    }
    return true;
}

RayTracer::Config common::parseRayTracerConfig(const AppConfig& config)
{
    RayTracer::Config traceConfig;
    traceConfig.detail = config.detail;
    traceConfig.occlusionRayCount = config.occlusionRayCount;
    traceConfig.softshadowRayCount = config.softshadowRayCount;
    traceConfig.threads = config.threads;
    traceConfig.width = config.width;
    traceConfig.height = config.height;
    return traceConfig;
}

bool common::writeToTGA(const Image& image, const char* filename)
{

    auto tga = targa::encode(image);
    File f = File::openW(filename);
    if (f.isValid())
    {
        f.write(tga.data(), tga.size());
    }
    return f.isValid();
}
