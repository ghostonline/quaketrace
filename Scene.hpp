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

    struct Plane
    {
        math::Vec3f origin;
        math::Vec3f normal;
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
    std::vector<Plane> planes;

    static void initDefault(Scene* scene);
};
