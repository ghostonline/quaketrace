#pragma once

#include "Scene.hpp"

struct BspLoader
{
    static const Scene createSceneFromBsp(const void* data, int size);
};
