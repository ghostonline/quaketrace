#include "QuakeTraceApp.hpp"
#include "SDL.h"
#include "FrameBuffer.hpp"
#include <memory>
#include <cstdint>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Math.hpp"
#include "Texture.hpp"
#include "Font.hpp"
#include "Geometry.hpp"
#include "AssetHelper.hpp"
#include "GraphicTool.hpp"
#include "Scene.hpp"
#include "Assert.hpp"
#include "Collision3D.hpp"
#include "Ray.hpp"
#include "Util.hpp"
#include "BspLoader.hpp"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;
const int DETAIL_LEVEL = 1;

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;
const Color COLOR_BACKGROUND {0x22/255.0f, 0x22/255.0f, 0x22/255.0f};

int QuakeTraceApp::breakX = -1;
int QuakeTraceApp::breakY = -1;

using namespace std;

#define DEFAULT_SCENE 0

struct ARGBCanvas
{
    ARGBCanvas(int width, int height) : width(width), height(height), pixels(width * height * PIXEL_SIZE) {}

    static const int PIXEL_SIZE = 4;
    int width;
    int height;
    std::vector<uint8_t> pixels;
};

void QuakeTraceApp::setIconFromAsset(SDL_Window* window, AssetHelper::ID id)
{
    auto iconSrc = AssetHelper::loadSurface(id);
    auto icon = SDL_ConvertSurfaceFormat(iconSrc, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(iconSrc);
    uint32_t* pixels = reinterpret_cast<uint32_t*>(icon->pixels);
    int pitch = icon->pitch >> 2;
    for (int x = icon->w - 1; x >= 0; --x)
    {
        for (int y = icon->h - 1; y >= 0; --y)
        {
            if (pixels[x + y * pitch] == COLOR_TRANSPARENT)
            {
                pixels[x + y * pitch] = 0;
            }
        }
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

void QuakeTraceApp::runUntilFinished()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("QuakeTrace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        SDL_Log("Failed to create a window: %s\n", SDL_GetError());
        return;
    }
    setIconFromAsset(window, AssetHelper::ICON);

    auto fb = FrameBuffer::createFromWindow(window);
    ARGBCanvas canvas(fb->getWidth(), fb->getHeight());

    auto font = Font::create();
#if DEFAULT_SCENE
    Scene scene;
    Scene::initDefault(&scene);
#else
    int mapDataSize = 0;
    const void* mapData = AssetHelper::getRaw(AssetHelper::TESTMAP, &mapDataSize);
    Scene scene = BspLoader::createSceneFromBsp(mapData, mapDataSize);
#endif
    // Correct for aspect ratio
    scene.camera.halfViewAngles.y *= SCREEN_HEIGHT / static_cast<float>(SCREEN_WIDTH);

    bool finished = false;
    int mouseX = 0, mouseY = 0;
    bool refreshCanvas = true;
    bool updateMouse = true;
    bool updateScene = false;
    uint32_t renderTime = 0;
    while (!finished)
    {
        // Process all events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            const bool quitEvent =
                (event.type == SDL_QUIT) || // Window close or ALT+F4
                (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE); // Esc KEY
            if (quitEvent)
            {
                finished = true;
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                updateMouse = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                breakX = mouseX;
                breakY = mouseY;
                refreshCanvas = true;
            }
        }

        if (refreshCanvas)
        {
            uint32_t renderStart = SDL_GetTicks();
            renderScene(scene, DETAIL_LEVEL, &canvas);
            renderTime = SDL_GetTicks() - renderStart;
            refreshCanvas = false;
            updateScene = true;
        }
        else
        {
            SDL_Delay(250);
        }

        if (updateMouse || updateScene)
        {
            {
                uint8_t* fbPixels = static_cast<uint8_t*>(fb->get());
                const int rowLength = canvas.width * ARGBCanvas::PIXEL_SIZE;

                if (fb->getPitch() == rowLength)
                {
                    // Just copy over the full buffer in one go
                    memcpy(fbPixels, canvas.pixels.data(), canvas.pixels.size());
                }
                else
                {
                    for (int ii = 0; ii < canvas.height; ++ii)
                    {
                        uint8_t* fbRow = fbPixels + (ii * fb->getPitch());
                        uint8_t* canvasRow = canvas.pixels.data() + (ii * canvas.width);
                        memcpy(fbRow, canvasRow, rowLength);
                    }
                }
                updateScene = false;
            }

            // Display rendertime
            {
                char renderTimeStr[50];
                sprintf(renderTimeStr, "%d ms", renderTime);
                font->blitString(fb, renderTimeStr, 0, 0);
            }

            // Display mouse x, y
            {
                char renderMouseStr[50];
                sprintf(renderMouseStr, "%d, %d", mouseX, mouseY);
                font->blitString(fb, renderMouseStr, 0, 10);
                updateMouse = false;
            }
        }


        fb->flip();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}

void QuakeTraceApp::renderScene(const Scene& scene, const int detailLevel, ARGBCanvas* canvas)
{
    const float sampleWidth = 1.0f / detailLevel;
    const float sampleHeight = 1.0f / detailLevel;
    const float halfSampleWidth = sampleWidth / 2.0f;
    const float halfSampleHeight = sampleHeight / 2.0f;
    std::vector<math::Vec2f> sampleOffsets;
    for (int dx = detailLevel - 1; dx >= 0; --dx)
    {
        for (int dy = detailLevel - 1; dy >= 0; --dy)
        {
            math::Vec2f offset = {
                sampleWidth * dx + halfSampleWidth,
                sampleHeight * dy + halfSampleHeight,
            };
            sampleOffsets.push_back(offset);
        }
    }
    const math::Vec2f fbSize(static_cast<float>(canvas->width), static_cast<float>(canvas->height));

    uint8_t* pixels = canvas->pixels.data();
    // TODO: Parallelize
    for (int x = canvas->width - 1; x >= 0; --x)
    {
        for (int y = canvas->height - 1; y >= 0; --y)
        {
            if (breakX == x && breakY == y)
            {
                breakX = breakY = -1;
                SDL_TriggerBreakpoint();
            }
            const int baseIdx = (x + y * canvas->width) * ARGBCanvas::PIXEL_SIZE;
            uint32_t* pixel = reinterpret_cast<uint32_t*>(&pixels[baseIdx]);
            Color aggregate(0.0f);

            for (int ii = util::lastIndex(sampleOffsets); ii >= 0; --ii)
            {
                const float sampleX = x + sampleOffsets[ii].x;
                const float sampleY = y + sampleOffsets[ii].y;
                const float normX = (sampleX / static_cast<float>(canvas->width) - 0.5f) * 2.0f;
                const float normY = (sampleY / static_cast<float>(canvas->height) - 0.5f) * -2.0f;
                Color color = renderPixel(scene, normX, normY);
                aggregate += color / static_cast<float>(sampleOffsets.size());
            }

            *pixel = Color::asARGB(aggregate);
        }
    }
}

const Color QuakeTraceApp::renderPixel(const Scene& scene, float x, float y)
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

    Color color = COLOR_BACKGROUND;
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
        lighted = mat.lighted;
        color = scene.getTexturePixel(mat, infoPolygon.pos);
        hitInfo = infoPolygon;
    }

    float lightLevel = 0.0f;
    if (lighted)
    {
        lightLevel = scene.lighting.calcLightLevel(hitInfo.pos, hitInfo.normal, scene);
    }
    else
    {
        lightLevel = 1.0f;
    }

    return color * lightLevel;
}
