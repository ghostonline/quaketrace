#pragma once

#include "AssetHelper.hpp"

struct SDL_Window;

class QuakeTraceApp
{
public:
    static void runUntilFinished();
    static void setIconFromAsset(SDL_Window* window, AssetHelper::ID id);
};
