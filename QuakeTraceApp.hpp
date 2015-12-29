#pragma once

#include "AssetHelper.hpp"
#include <cstdint>

struct SDL_Window;
struct Scene;
class FrameBuffer;

class QuakeTraceApp
{
public:
    static void runUntilFinished();

private:
    static void setIconFromAsset(SDL_Window* window, AssetHelper::ID id);
    static void renderScene(const Scene& scene, FrameBuffer* fb);
    static std::uint32_t renderPixel(const Scene& scene, int x, int y);
};
