#pragma once

#include "AssetHelper.hpp"

struct SDL_Window;

class QuakeTraceApp
{
public:
    static int runUntilFinished(int argc, char const * const * const argv);

private:
    static void setIconFromAsset(SDL_Window* window, AssetHelper::ID id);
};
