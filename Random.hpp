#pragma once

#include <cstdlib>

struct Random
{
    static void setGlobalSeed(unsigned int seed)
    {
        std::srand(seed);
    }

    static const float randFloat()
    {
        return std::rand() / static_cast<float>(RAND_MAX);
    }

    static const int randInt()
    {
        return std::rand();
    }

    static const float rand(float max)
    {
        return randFloat() * max;
    }

    static const float rand(float min, float max)
    {
        return min + rand(max - min);
    }
};
