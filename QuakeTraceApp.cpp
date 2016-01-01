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

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;
const int FRAME_RATE = 1000 / 60; // ticks per frame

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;
const std::uint32_t COLOR_BACKGROUND = 0xFF222222;

using namespace std;
using namespace math;

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
    uint32_t lastTick = SDL_GetTicks();
#if SHOW_FPS
    int frames = 0;
    int frameTimeout = 0;
    int framesPerSecond = 0;
#endif

    while (!finished)
    {
        // Calculate dT in seconds
        uint32_t dTicks = SDL_GetTicks() - lastTick;
#if SHOW_FPS
        frames += 1;
#endif

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

        while (dTicks >= FRAME_RATE)
        {
            lastTick += FRAME_RATE;
            dTicks -= FRAME_RATE;

#if SHOW_FPS
            frameTimeout += FRAME_RATE;
            if (frameTimeout > 1000)
            {
                framesPerSecond = frames;
                frames = 0;
                frameTimeout -= 1000;

                if (framesPerSecond < 0) { framesPerSecond = 0; }
                else if (framesPerSecond >= 10000) { framesPerSecond = 99999; }
            }
#endif

            fb->flip();
        }

        renderScene(scene, fb);

#if SHOW_FPS
        // Render FPS
        {
            char fpscounter[11];
            sprintf(fpscounter, "fps: %d", framesPerSecond);
            font->blitString(fb, fpscounter, 0, 0);
        }
#endif

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
            float normY = ((y + 0.5f) / static_cast<float>(fb->getHeight()) - 0.5f) * 2.0f;
            *pixel = renderPixel(scene, normX, normY);
        }
    }
}

std::uint32_t QuakeTraceApp::renderPixel(const Scene& scene, float x, float y)
{
    const Scene::Camera& camera = scene.camera;

    Vec3f dir = camera.direction;
    dir += camera.right * (x * camera.halfViewAngles.x);
    dir += camera.up * (y * camera.halfViewAngles.y);
    Vec3f::normalize(&dir);

    float minDist = camera.far;
    std::uint32_t minColor = COLOR_BACKGROUND;
    for (const Scene::Sphere& sphere : scene.spheres)
    {
        auto relativeOrigin = sphere.origin - camera.origin;
        float dot = Vec3f::dot(relativeOrigin, dir);
        auto projection = dir * dot;
        if (Vec3f::distance2(projection, relativeOrigin) > math::squared(sphere.radius)) { continue; }
        float distance = Vec3f::distance(projection, relativeOrigin);

        float penetration = std::sqrt(math::squared(sphere.radius) - math::squared(distance));
        float dist = dot - penetration;
        if (minDist > dist)
        {
            minColor = Color::asUint(sphere.color);
        }
    }

    return minColor;
}
