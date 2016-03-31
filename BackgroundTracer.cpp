#include "BackgroundTracer.hpp"

BackgroundTracer::BackgroundTracer(const RayTracer::Config& config)
: engine(config)
, canvas(config.width, config.height, Image::FORMAT_ARGB)
{}

void BackgroundTracer::startTrace(const Scene& scene)
{
    engine.trace(scene, &canvas);
}
