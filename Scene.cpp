#include "Scene.hpp"
#include "Assert.hpp"

void Scene::initDefault(Scene* scene)
{
    ASSERT(scene != nullptr);

    // Camera at (0,0,-10), pointing upwards
    scene->camera.origin.set(0, 0, -10);
    scene->camera.direction.set(0, 0, 1);
    scene->camera.up.set(0, 1, 0);
    scene->camera.viewDimensions.set(20.0f, 20.0f);
    scene->camera.near = 10;

    // Sphere
    scene->spheres.push_back({ math::Vec3f(15.0f, 15.0f, 20.0f), 8.0, Color(1.0f, 0.0f, 0.0f) });
}
