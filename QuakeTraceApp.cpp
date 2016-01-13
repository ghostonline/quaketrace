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

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;
const Color COLOR_BACKGROUND {0x22/255.0f, 0x22/255.0f, 0x22/255.0f};

int QuakeTraceApp::breakX = -1;
int QuakeTraceApp::breakY = -1;

using namespace std;

#define SHOW_FPS _DEBUG

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
    auto font = Font::create();
    Scene scene;
    Scene::initDefault(&scene);

    bool finished = false;
    int mouseX = 0, mouseY = 0;
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
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                breakX = mouseX;
                breakY = mouseY;
            }
        }

        uint32_t renderStart = SDL_GetTicks();
        renderScene(scene, fb);
        uint32_t renderTime = SDL_GetTicks() - renderStart;

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
        }


        fb->flip();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}

void QuakeTraceApp::renderScene(const Scene& scene, FrameBuffer* fb)
{
    uint8_t* pixels = reinterpret_cast<uint8_t*>(fb->get());
    // TODO: Parallelize
    for (int x = fb->getWidth() - 1; x >= 0; --x)
    {
        for (int y = fb->getHeight() - 1; y >= 0; --y)
        {
            if (breakX == x && breakY == y)
            {
                breakX = breakY = -1;
                SDL_TriggerBreakpoint();
            }
            const int baseIdx = x * fb->getPixelSize() + y * fb->getPitch();
            uint32_t* pixel = reinterpret_cast<uint32_t*>(&pixels[baseIdx]);
            float normX = ((x + 0.5f) / static_cast<float>(fb->getWidth()) - 0.5f) * 2.0f;
            float normY = ((y + 0.5f) / static_cast<float>(fb->getHeight()) - 0.5f) * -2.0f;
            *pixel = renderPixel(scene, normX, normY);
        }
    }
}

std::uint32_t QuakeTraceApp::renderPixel(const Scene& scene, float x, float y)
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

    collision3d::Hit infoSphere, infoPlane, infoTriangle;
    infoSphere.t = scene.camera.far;
    int sphereHitIdx = collision3d::raycastSpheres(pixelRay, infoSphere.t, scene.spheres, &infoSphere);
    infoPlane.t = infoSphere.t;
    int planeHitIdx = collision3d::raycastPlanes(pixelRay, infoSphere.t, scene.planes, &infoPlane);
    int triangleHitIdx = collision3d::raycastTriangles(pixelRay, infoPlane.t, scene.triangles, &infoTriangle);

    Color color = COLOR_BACKGROUND;
    collision3d::Hit hitInfo;
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

    float lightLevel = 0.0f;
    for (int ii = util::lastIndex(scene.lights); ii >= 0; --ii)
    {
        const Scene::Light& light = scene.lights[ii];
        Ray lightRay{ hitInfo.pos, light.origin - hitInfo.pos };
        float rayLength = math::length(lightRay.dir);
        lightRay.dir /= rayLength;
        bool hit = false
            || collision3d::raycastSpheres(lightRay, rayLength, scene.spheres) > -1
            || collision3d::raycastPlanes(lightRay, rayLength, scene.planes) > -1
            || collision3d::raycastTriangles(lightRay, rayLength, scene.triangles) > -1
        ;

        lightLevel = hit ? 0.0f : math::dot(lightRay.dir, hitInfo.normal);
    }

    float colorScale = math::clamp01(lightLevel + scene.ambientLightFactor);
    return Color::asARGB(color * colorScale);
}
