#pragma once

#include "Math.hpp"
#include "Color.hpp"
#include <vector>

class FrameBuffer;

struct Scene
{
    static const int DIRECTIONAL_RAY_LENGTH = 1000;

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

    struct Triangle
    {
        math::Vec3f a;
        math::Vec3f b;
        math::Vec3f c;

        Color color;
    };

    struct DirectionalLight
    {
        math::Vec3f normal;
        float intensity;
    };

    struct Light
    {
        math::Vec3f origin;
        float strength;
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
    std::vector<Triangle> triangles;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Light> lights;

    float ambientLightFactor;

    static void initDefault(Scene* scene);
};
