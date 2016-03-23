#pragma once

#include "AssetHelper.hpp"
#include "Color.hpp"

struct SDL_Window;
struct Scene;
struct Ray;
struct Image;

class QuakeTraceApp
{
public:
    static int runUntilFinished(int argc, char const * const * const argv);
    static const Color renderPixel(const Scene& scene, float x, float y);

private:
    static void setIconFromAsset(SDL_Window* window, AssetHelper::ID id);
    static void renderScene(const Scene& scene, const int detailLevel, Image* fb);

    static int breakX, breakY;
};
