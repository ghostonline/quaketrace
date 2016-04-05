#include "BackgroundTracer.hpp"
#include "Assert.hpp"
#include "Scene.hpp"

BackgroundTracer::BackgroundTracer(const RayTracer::Config& config)
: running(false)
, engine(config)
, canvas(config.width, config.height, Image::FORMAT_ARGB)
{
}

BackgroundTracer::~BackgroundTracer()
{
    running = false;
    if (thread.joinable()) { thread.join(); }
}

void BackgroundTracer::startTrace(const Scene& scene, const Camera& camera)
{
    ASSERT(this->scene == nullptr);
    this->scene.reset(new Scene(scene));
    this->camera.reset(new Camera(camera));

    if (!running)
    {
        if (thread.joinable()) { thread.join(); }
        running = true;
        thread = std::thread(threadRunner, this);
    }
}

void BackgroundTracer::traceAsync()
{
    std::this_thread::yield();
    if (scene)
    {
        engine.trace(*scene, *camera, &canvas);
        scene.reset();
        camera.reset();
    }
    running = false;
}
