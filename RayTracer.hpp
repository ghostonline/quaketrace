#include "Image.hpp"
#include "Color.hpp"

struct Scene;

class RayTracer
{
    friend struct RayContext;

public:
    struct Config
    {
        int width;
        int height;
        int detail;

        int softshadowRayCount;
        int occlusionRayCount;

        int threads;
    };

    RayTracer(const Config& config)
    : config(config)
    , breakX(-1)
    , breakY(-1)
    , progress(0.0f)
    {}

    void setBreakPoint(int x, int y) { breakX = x; breakY = y; }
    void resetBreakPoint() { breakX = breakY = -1; }

    float getProgress() const { return progress; }
    const Image trace(const Scene& scene);
    void trace(const Scene& scene, Image* target);

private:
    const Color renderPixel(const Scene& scene, float x, float y) const;

    Config config;
    int breakX, breakY;
    float progress;
};
