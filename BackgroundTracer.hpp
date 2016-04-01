#pragma once

#include "RayTracer.hpp"
#include <memory>
#include <thread>

class BackgroundTracer
{
public:
    BackgroundTracer(const RayTracer::Config& config);
    ~BackgroundTracer();

    void startTrace(const Scene& scene);
    float getProgress() { return 0; }
    const Image& getCanvas() const { return canvas; }

private:
    static void threadRunner(BackgroundTracer* self) { self->traceAsync(); }
    void traceAsync();

    bool running;
    RayTracer engine;
    std::unique_ptr<Scene> scene;
    Image canvas;

    std::thread thread;
};