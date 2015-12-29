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
        math::Vec3f right;
        math::Vec2f halfViewAngles;

        float near;
        float far;
    };

    Camera camera;
    std::vector<Sphere> spheres;

    static void initDefault(Scene* scene);
};
