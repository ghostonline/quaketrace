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

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;
const int FRAME_RATE = 1000 / 60; // ticks per frame

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;
const std::uint32_t COLOR_BACKGROUND = 0xFF222222;

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
        }

        // Display rendertime
        {
            uint32_t renderTime = SDL_GetTicks();
            renderScene(scene, fb);
            char renderTimeStr[50];
            sprintf(renderTimeStr, "%d ms", SDL_GetTicks() - renderTime);
            font->blitString(fb, renderTimeStr, 0, 0);
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

    if (triangleHitIdx > -1)
    {
        return Color::asUint(scene.triangles[triangleHitIdx].color);
    }

    if (planeHitIdx > -1)
    {
        const Scene::Plane* minPlane = &scene.planes[planeHitIdx];
        float dot = math::dot(pixelRay.dir, -minPlane->normal);
        float shade = math::clamp01(dot) * 0.9f + 0.1f;
        return Color::asUint(minPlane->color * shade);
    }

    if (sphereHitIdx > -1)
    {
        float shade = math::clamp01(math::dot(infoSphere.normal, pixelRay.dir)) * 0.9f + 0.1f;
        return Color::asUint(scene.spheres[sphereHitIdx].color * shade);
    }

    return COLOR_BACKGROUND;
}
