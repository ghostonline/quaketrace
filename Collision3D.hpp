#pragma once

#include "Vec3.hpp"
#include "Scene.hpp"

struct Ray;

namespace collision3d
{
    struct Hit
    {
        math::Vec3f pos;
        math::Vec3f normal;
        float t;
    };

    int raycastSpheres(const Ray& ray, float maxDist, const std::vector<Scene::Sphere>& spheres, Hit* hitResult = nullptr);
    int raycastPlanes(const Ray& ray, float maxDist, const std::vector<Scene::Plane>& planes, Hit* hitResult = nullptr);
    int raycastTriangles(const Ray& ray, float maxDist, const std::vector<Scene::Triangle>& triangles, Hit* hitResult = nullptr);
}
