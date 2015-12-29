#include "Scene.hpp"
#include "Assert.hpp"
#include <cmath>

void Scene::initDefault(Scene* scene)
{
    ASSERT(scene != nullptr);

    // Camera at (0,0,-10), pointing towards positive Z
    scene->camera.origin.set(0, 0, -10);
    scene->camera.direction.set(0, 0, 1);
    scene->camera.up.set(0, 1, 0);
    scene->camera.right.set(1, 0, 0);

    scene->camera.near = 10.0f;
    scene->camera.far = 1000.0f;
    float maxXAngle = std::atan(10.0f / scene->camera.near);
    float maxYAngle = std::atan(10.0f / scene->camera.near);
    scene->camera.halfViewAngles.set(maxXAngle, maxYAngle);

    // Sphere
    scene->spheres.push_back({ math::Vec3f(15.0f, 15.0f, 20.0f), 8.0, Color(1.0f, 0.0f, 0.0f) });
    scene->spheres.push_back({ math::Vec3f(15.0f, 10.0f, 15.0f), 6.0, Color(0.0f, 1.0f, 0.0f) });
}
