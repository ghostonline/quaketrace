#include "RayTracer.hpp"
#include "Scheduler.hpp"
#include "Util.hpp"
#include "Ray.hpp"
#include "Collision3D.hpp"

struct RayInput
{
    int x, y, pixelIdx;
    const bool breakDebugger;
};

struct RayContext
{
    int canvasWidth, canvasHeight;
    const RayTracer& engine;
    const RayTracer::Config& config;
    const std::vector<math::Vec2f>& sampleOffsets;

    const Color process(RayInput in) const;
};

const Image RayTracer::trace(const RayTracer::Config& config) const
{
    Image canvas(config.width, config.height, Image::FORMAT_ARGB);

    const float sampleWidth = 1.0f / config.detail;
    const float sampleHeight = 1.0f / config.detail;
    const float halfSampleWidth = sampleWidth / 2.0f;
    const float halfSampleHeight = sampleHeight / 2.0f;
    std::vector<math::Vec2f> sampleOffsets;
    for (int dx = config.detail - 1; dx >= 0; --dx)
    {
        for (int dy = config.detail - 1; dy >= 0; --dy)
        {
            math::Vec2f offset = {
                sampleWidth * dx + halfSampleWidth,
                sampleHeight * dy + halfSampleHeight,
            };
            sampleOffsets.push_back(offset);
        }
    }
    const math::Vec2f fbSize(static_cast<float>(canvas.width), static_cast<float>(canvas.height));

    RayContext context = {canvas.width, canvas.height, *this, config, sampleOffsets};

    uint8_t* pixels = canvas.pixels.data();
    std::vector<RayInput> input;
    for (int x = canvas.width - 1; x >= 0; --x)
    {
        for (int y = canvas.height - 1; y >= 0; --y)
        {
            const int baseIdx = (x + y * canvas.width) * canvas.getPixelSize();
            input.push_back({x, y, baseIdx, breakX == x && breakY == y});
        }
    }

    Scheduler scheduler(config.threads);
    auto output = scheduler.schedule<RayInput, Color, RayContext>(input, context);
    for (int ii = util::lastIndex(output); ii >= 0; --ii)
    {
        const auto& color = output[ii];
        const int pixelIdx = input[ii].pixelIdx;
        uint32_t* pixel = reinterpret_cast<uint32_t*>(&pixels[pixelIdx]);
        *pixel = Color::asARGB(color);
    }

    return canvas;
}

const Color RayContext::process(RayInput in) const
{
    if (in.breakDebugger)
    {
        SDL_TriggerBreakpoint();
    }

    Color aggregate(0.0f);

    for (int ii = util::lastIndex(sampleOffsets); ii >= 0; --ii)
    {
        const float sampleX = in.x + sampleOffsets[ii].x;
        const float sampleY = in.y + sampleOffsets[ii].y;
        const float normX = (sampleX / static_cast<float>(canvasWidth) - 0.5f) * 2.0f;
        const float normY = (sampleY / static_cast<float>(canvasHeight) - 0.5f) * -2.0f;
        Color color = engine.renderPixel(config, normX, normY);
        aggregate += color / static_cast<float>(sampleOffsets.size());
    }
    return aggregate;
}

const Color RayTracer::renderPixel(const Config& config, float x, float y) const
{
    Ray pixelRay;
    {
        const Scene::Camera& camera = scene.camera;

        math::Vec3f dir = camera.direction;
        dir += camera.right * (x * camera.halfViewAngles.x);
        dir += camera.up * (y * camera.halfViewAngles.y);
        math::normalize(&dir);

        pixelRay.origin = camera.origin;
        pixelRay.dir = dir;
    }

    collision3d::Hit infoSphere, infoPlane, infoTriangle, infoPolygon;
    infoSphere.t = scene.camera.far;
    int sphereHitIdx = collision3d::raycastSpheres(pixelRay, infoSphere.t, scene.spheres, &infoSphere);
    infoPlane.t = infoSphere.t;
    int planeHitIdx = collision3d::raycastPlanes(pixelRay, infoSphere.t, scene.planes, &infoPlane);
    int triangleHitIdx = collision3d::raycastTriangles(pixelRay, infoPlane.t, scene.triangles, &infoTriangle);
    int polygonHitIdx = collision3d::raycastConvexPolygons(pixelRay, infoPlane.t, scene.polygons, &infoPolygon);

    Color color;
    collision3d::Hit hitInfo;
    bool lighted = true;
    if (triangleHitIdx > -1)
    {
        color = scene.triangles[triangleHitIdx].color;
        hitInfo = infoTriangle;
    }
    else if (planeHitIdx > -1)
    {
        color = scene.planes[planeHitIdx].color;
        hitInfo = infoPlane;
    }
    else if (sphereHitIdx > -1)
    {
        color = scene.spheres[sphereHitIdx].color;
        hitInfo = infoSphere;
    }
    else if (polygonHitIdx > -1)
    {
        const Scene::Material& mat = scene.polygons[polygonHitIdx].material;
        Scene::TexturePixel pixel;
        if (mat.useSkyShader)
        {
            pixel = scene.getSkyPixel(mat, pixelRay, {config.width, config.height});
        }
        else
        {
            pixel = scene.getTexturePixel(mat, infoPolygon.pos);
        }
        color = pixel.color;
        lighted = !pixel.fullbright;
        hitInfo = infoPolygon;
    }

    float lightLevel = 0.0f;
    if (lighted)
    {
        lightLevel = scene.lighting.calcLightLevel(hitInfo.pos, hitInfo.normal, scene, config.softshadowRayCount, config.occlusionRayCount);
    }
    else
    {
        lightLevel = 1.0f;
    }
    
    return color * lightLevel;
}
