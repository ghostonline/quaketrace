#pragma once

#include "RayTracer.hpp"

class BackgroundTracer
{
public:
    BackgroundTracer(const RayTracer::Config& config);

    void startTrace(const Scene& scene);
    float getProgress() { return 0; }
    const Image& getCanvas() const { return canvas; }

private:
    RayTracer engine;
    Image canvas;
};