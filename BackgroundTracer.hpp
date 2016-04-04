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
    float getProgress() { return engine.getProgress(); }
    const Image& getCanvas() const { return canvas; }
    bool isTracing() const { return running; }
    void setBreakPoint(int x, int y) { engine.setBreakPoint(x, y); }
    void resetBreakPoint() { engine.resetBreakPoint(); }

private:
    static void threadRunner(BackgroundTracer* self) { self->traceAsync(); }
    void traceAsync();

    bool running;
    RayTracer engine;
    std::unique_ptr<Scene> scene;
    Image canvas;

    std::thread thread;
};