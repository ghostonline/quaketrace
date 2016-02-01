#pragma once

#include "AssetHelper.hpp"
#include "Color.hpp"

struct SDL_Window;
struct Scene;
struct Ray;
struct ARGBCanvas;

class QuakeTraceApp
{
public:
    static void runUntilFinished();

private:
    static void setIconFromAsset(SDL_Window* window, AssetHelper::ID id);
    static void renderScene(const Scene& scene, const int detailLevel, ARGBCanvas* fb);
    static const Color renderPixel(const Scene& scene, float x, float y);

    static int breakX, breakY;
};
