#include "Scene.hpp"
#include "Assert.hpp"
#include <cmath>
#include "Math.hpp"

void Scene::initDefault(Scene* scene)
{
    ASSERT(scene != nullptr);

    pointCamera(&scene->camera, {0, 0, 30}, {0, 0, -1}, {0, 1, 0});

    // Spheres
    scene->spheres.push_back({ math::Vec3f(15.0f, 15.0f, -20.0f), 8.0, Color(1.0f, 0.0f, 0.0f) });
    scene->spheres.push_back({ math::Vec3f(15.0f, 5.0f, -5.0f), 6.0, Color(0.0f, 1.0f, 0.0f) });

    // Ground plane
    scene->planes.push_back({ math::Vec3f(0, -10, 0), math::Vec3f(0, 1, 0), Color(0.0f, 0.0f, 1.0f) });

    // Random triangle
    scene->triangles.push_back({ math::Vec3f(-5, 0, 0), math::Vec3f(5, 0, 0), math::Vec3f(0, 5, 0), Color(0.5f, 0.5f, 1.0f) });

    // Polygon
    ConvexPolygon polygon;
    polygon.color = Color(1.0f, 0.5f, 0.5f);
    polygon.vertices.push_back(math::Vec3f(-3, 7, -1));
    polygon.vertices.push_back(math::Vec3f(0, 5, -1));
    polygon.vertices.push_back(math::Vec3f(3, 7, -1));
    polygon.vertices.push_back(math::Vec3f(2, 10, -1));
    polygon.vertices.push_back(math::Vec3f(-2, 10, -1));
    scene->polygons.push_back(polygon);

    // Directional lights
    scene->directionalLights.push_back({ math::Vec3f(0, -1, 0), 0.3f});

    // Point lights
    scene->lights.push_back({ math::Vec3f(0, 30, -5), 3000});

    scene->ambientLightFactor = 0.3f;
}

void Scene::pointCamera(Camera* camera, const math::Vec3f& pos, const math::Vec3f& forward, const math::Vec3f& up)
{
    camera->origin = pos;
    camera->direction = forward;
    camera->up = up; // General up direction
    ASSERT(camera->up != camera->direction);
    camera->right = math::cross(camera->direction, camera->up);
    camera->up = math::cross(camera->right, camera->direction);

    math::normalize(&camera->direction);
    math::normalize(&camera->up);
    math::normalize(&camera->right);

    camera->near = 10.0f;
    camera->far = 10000.0f;

    const float fov = 60;
    camera->halfViewAngles.set(
                               std::tan(math::deg2rad(fov)) / 2.0f,
                               std::tan(math::deg2rad(fov)) / 2.0f
                               );
}
