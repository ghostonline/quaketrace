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

    bool raySceneCollision(const Ray& ray, float maxDist, const Scene& scene);
    int raycastSpheres(const Ray& ray, float maxDist, const std::vector<Scene::Sphere>& spheres, Hit* hitResult = nullptr);
    int raycastPlanes(const Ray& ray, float maxDist, const std::vector<Scene::Plane>& planes, Hit* hitResult = nullptr);
    int raycastTriangles(const Ray& ray, float maxDist, const std::vector<Scene::Triangle>& triangles, Hit* hitResult = nullptr);
    int raycastConvexPolygons(const Ray& ray, float maxDist, const std::vector<Scene::ConvexPolygon>& polygons, Hit* hitResult = nullptr);
}

inline bool collision3d::raySceneCollision(const Ray& ray, float maxDist, const Scene& scene)
{
    return false
    || collision3d::raycastSpheres(ray, maxDist, scene.spheres) > -1
    || collision3d::raycastPlanes(ray, maxDist, scene.planes) > -1
    || collision3d::raycastTriangles(ray, maxDist, scene.triangles) > -1
    || collision3d::raycastConvexPolygons(ray, maxDist, scene.polygons) > -1
    ;
}
