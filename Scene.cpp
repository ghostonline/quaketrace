#include "Scene.hpp"
#include "Assert.hpp"
#include <cmath>
#include "Math.hpp"

void Scene::initDefault(Scene* scene)
{
    ASSERT(scene != nullptr);

    scene->camera.origin.set(0, 0, 30);
    scene->camera.direction.set(0, 0, -1);
    scene->camera.up.set(0, 1, 0); // General up direction
    ASSERT(scene->camera.up != scene->camera.direction);
    scene->camera.right = math::cross(scene->camera.direction, scene->camera.up);
    scene->camera.up = math::cross(scene->camera.right, scene->camera.direction);

    scene->camera.near = 10.0f;
    scene->camera.far = 10000.0f;

    const float fov = 60;
    scene->camera.halfViewAngles.set(
        std::tan(math::deg2rad(fov)) / 2.0f,
        std::tan(math::deg2rad(fov)) / 2.0f
    );

    // Spheres
    scene->spheres.push_back({ math::Vec3f(15.0f, 15.0f, -20.0f), 8.0, Color(1.0f, 0.0f, 0.0f) });
    scene->spheres.push_back({ math::Vec3f(15.0f, 5.0f, -5.0f), 6.0, Color(0.0f, 1.0f, 0.0f) });

    // Ground plane
    scene->planes.push_back({ math::Vec3f(0, -10, 0), math::Vec3f(0, 1, 0), Color(0.0f, 0.0f, 1.0f) });

    // Random triangle
    scene->triangles.push_back({ math::Vec3f(-5, 0, 0), math::Vec3f(5, 0, 0), math::Vec3f(0, 5, 0), Color(0.5f, 0.5f, 1.0f) });

    // Directional lights
    scene->directionalLights.push_back({ math::Vec3f(0, -1, 0), 0.3f});

    // Point lights
    scene->lights.push_back({ math::Vec3f(0, 30, -5), 3000});

    scene->ambientLightFactor = 0.1f;
}
