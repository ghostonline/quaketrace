#pragma once

#include "Math.hpp"
#include "Color.hpp"
#include <vector>

class FrameBuffer;

struct Scene
{
    struct Sphere
    {
        math::Vec3f origin;
        float radius;
        Color color;
    };

    struct Camera
    {
        math::Vec3f origin;
        math::Vec3f direction;
        math::Vec3f up;
        math::Vec2f viewDimensions;

        float near;

    };

    Camera camera;
    std::vector<Sphere> spheres;

    static void initDefault(Scene* scene);
};
