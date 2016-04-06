#pragma once
#include "RayTracer.hpp"

struct Scene;
struct AppConfig;
struct Image;

namespace common {
    bool loadBSP(const char* filename, Scene* scene, int screenWidth, int screenHeight);
    RayTracer::Config parseRayTracerConfig(const AppConfig& config);
    bool writeToTGA(const Image& image, const char* filename);
}
