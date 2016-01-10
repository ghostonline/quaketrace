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
    const Scene::Camera& camera = scene.camera;

    math::Vec3f dir = camera.direction;
    dir += camera.right * (x * camera.halfViewAngles.x);
    dir += camera.up * (y * camera.halfViewAngles.y);
    math::normalize(&dir);

    float minDist = camera.far;
    const Scene::Sphere* minSphere = nullptr;
    for (const Scene::Sphere& sphere : scene.spheres)
    {
        auto relativeOrigin = sphere.origin - camera.origin;
        float dot = math::dot(relativeOrigin, dir);
        auto projection = dir * dot;
        if (math::distance2(projection, relativeOrigin) > math::squared(sphere.radius)) { continue; }
        float distance = math::distance(projection, relativeOrigin);

        float penetration = std::sqrt(math::squared(sphere.radius) - math::squared(distance));
        float dist = dot - penetration;
        if (minDist > dist)
        {
            minSphere = &sphere;
            minDist = dist;
        }
    }

    const Scene::Plane* minPlane = nullptr;
    for (const Scene::Plane& plane : scene.planes)
    {
        float denom = math::dot(dir, plane.normal);
        // Only render plane if point moves toward front of the plane
        if (denom > math::APPROXIMATE_ZERO) { continue; }

        // Determine intersection time
        auto relativeOrigin = plane.origin - camera.origin;
        float t = math::dot(relativeOrigin, plane.normal) / denom;

        // Calculate intersection point relative to the camera
        auto intersection = dir * t;
        float dist = math::length(intersection);
        if (minDist > dist)
        {
            minDist = dist;
            minPlane = &plane;
        }
    }

    const Scene::Triangle* minTriangle = nullptr;
    for (const Scene::Triangle& triangle : scene.triangles)
    {
        // TODO: Precalculate this?
        math::Vec3f edgeAB = triangle.b - triangle.a;
        math::Vec3f edgeAC = triangle.c - triangle.a;
        math::Vec3f triangleNormal = math::cross(edgeAB, edgeAC);

        // Find intersection point with plane
        float denom = math::dot(dir, triangleNormal);
        if (denom > math::APPROXIMATE_ZERO) { continue; }
        auto relativeOrigin = triangle.a - camera.origin;
        float t = math::dot(relativeOrigin, triangleNormal) / denom;
        auto intersection = dir * t;

        float dist = math::length(intersection);
        if (minDist < dist) { continue; }

        // Small optimization
#if 0
        auto relativeIntersection = intersection + camera.origin - triangle.a;
#else
        auto relativeIntersection = intersection - relativeOrigin;
#endif

        // Check ray intersects in triangle boundaries (source: http://geomalgorithms.com/a04-_planes.html#Barycentric-Coordinate-Compute)
        const math::Vec3f& u = edgeAB;
        const math::Vec3f& v = edgeAC;
        const math::Vec3f& w = relativeIntersection;
        float uv = math::dot(u, v);
        float wv = math::dot(w, v);
        float vv = math::dot(v, v);
        float wu = math::dot(w, u);
        float uu = math::dot(u, u);
        float denom2 = math::squared(uv) - uu * vv;
        float sI = (uv*wv - vv*wu) / denom2;
        float tI = (uv*wu - uu*wv) / denom2;
        if (sI < 0.0f || tI < 0.0f || (sI + tI) > 1.0f) { continue; }

        minDist = dist;
        minTriangle = &triangle;
    }

    if (minTriangle)
    {
        return Color::asUint(minTriangle->color);
    }

    if (minPlane)
    {
        float dot = math::dot(dir, -minPlane->normal);
        float shade = math::clamp01(dot) * 0.9f + 0.1f;
        return Color::asUint(minPlane->color * shade);
    }

    if (minSphere)
    {
        const math::Vec3f minHitOrigin = dir * minDist + camera.origin;
        math::Vec3f hitNormal = minSphere->origin - minHitOrigin;
        math::normalize(&hitNormal);
        float shade = math::clamp01(math::dot(hitNormal, dir)) * 0.9f + 0.1f;
        return Color::asUint(minSphere->color * shade);
    }

    return COLOR_BACKGROUND;
}
