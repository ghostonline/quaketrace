#include "Scene.hpp"
#include "Image.hpp"
#include "Color.hpp"

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

    RayTracer(const Scene& scene)
    : scene(scene)
    , breakX(-1)
    , breakY(-1)
    {}

    void setBreakPoint(int x, int y) { breakX = x; breakY = y; }
    void resetBreakPoint() { breakX = breakY = -1; }
    
    const Image trace(const Config& config) const;

private:
    const Color renderPixel(const Config& config, float x, float y) const;

    const Scene scene;
    int breakX, breakY;
};
